#include "Inventory.h"
#include "ItemManager.h" 
#include "common.h" 
#include "Item.h" 
#include "TextureManager.h"
#include "GUI.h"
#include "GUIManager.h"
#include "ScrollManager.h"
#include "InputManager.h"
#include "CharacterController.h"
#include "GrabHook.h"

#define SHOW_INFO_TIMER 0.3f
#define INFO_SIZE sf::Vector2f(0.35f,0.7f)


PlayerInventory::PlayerInventory()
{
	GUIManager& guiMan = *GUIManager::GetInstance();
	ScrollManager& scroll = *ScrollManager::GetInstance();

	// Load UI data
	std::ifstream stream("ConfigFiles/GUI.dat/PlayerInv.dat");

	GUIImage::TextureSearchFunction = &TextureManager::GetTexture;
	if (stream.is_open())
	{
		guiMan.LoadPrefab(stream, 1);
		stream.close();
	}

	//create inv UI
	gui = (GUI*)guiMan.GetNodeByTag("invBackground");

	GUI* area = guiMan.GetNode(gui->GetChildren()[0]);
	GUIMaskingContainer* guiMaskingContainer = (GUIMaskingContainer*)guiMan.GetNode(area->GetChildren()[0]);
	GUIGridLayout* guiBackgroundLayout = (GUIGridLayout*)guiMan.GetNode(guiMaskingContainer->GetChildren()[0]);

	scroll.CreateNewStruct(guiMaskingContainer, guiBackgroundLayout, "PlayerInventory");

	for (int i = 0; i < guiBackgroundLayout->GetRowCount(); i++)
	{
		scroll.AddRow("PlayerInventory", "ConfigFiles/GUI.dat/playerInvItemSlot.dat");
	}

	gui->SetEnabled(false);
	informations = guiMan.GetNodeByTag("itemInformations");
	informations->SetEnabled(false);

}

// Adding item, one per one by default
void PlayerInventory::AddItem(Item* _item, unsigned int _quantity)
{
	auto itr = inventory.find(_item);
	if (itr == inventory.end())
	{
		inventory.insert(std::make_pair(_item, _quantity));
	}
	else
	{
		itr->second += _quantity;
	}
}

std::string TextEmplacer(sf::Text* _text, sf::RectangleShape* _shape, std::string _originalText) 
{
	//If the text globalBounds is bigger than the shape, make a \n in the text at the right position
    if (_text->getGlobalBounds().width > _shape->getGlobalBounds().width)
	{
		std::string realText = _originalText;
		sf::Text tempText(*_text);
		tempText.setString("");
		for (int i = 0; i < _originalText.size(); i++)
		{
			//Add characters one by one and check if the text is bigger than the shape
			tempText.setString(tempText.getString() + realText[i]);
			if(tempText.getGlobalBounds().width > _shape->getGlobalBounds().width)
			{
				// Search at the last space to not cut a word
				while (realText[i] != ' ')
				{
					i--;
				}
				realText.erase(i, 1);
				realText.insert(i, "\n");
				//cut text at i to recall TextEmplacer with the following text
				realText = realText.substr(i);
				_originalText = _originalText.substr(0, i);
				return std::string(_originalText + TextEmplacer(_text, _shape, realText));
			}
		}
	}
	return _originalText;
}

std::string GetTextRarity(Rarity _rarity) 
{
	switch (_rarity)
	{
		case Rarity::COMMON:
			return "Common";
		case Rarity::RARE:
			return "Rare";
		case Rarity::EPIC:
			return "Epic";
		case Rarity::MYTHIC:
			return "Mythic";
		case Rarity::LEGENDARY:
			return "Legendary";
	}
	return "";
}

void PlayerInventory::Update() 
{
	if(showingInventory)
	{
		sf::Vector2i mousePos = (sf::Vector2i)GameData::GetMousePos();

		GUIManager& guiMan = *GUIManager::GetInstance(); //Get gui

		GUIGridLayout* layout = ScrollManager::GetInstance()->GetStruct("PlayerInventory")->layout;

		bool hoveringSomething = false;
		int index = 0;
		auto inputs = InputManager::GetInstance();
		if (!onDrag) 
		{
			for (auto itr = inventory.begin(); itr != inventory.end(); itr++)
			{
				for (int j = 0; j < itr->second; j++)
				{
					GUI* tempSlot = guiMan.GetNode(layout->GetChildren()[index]);

					//Detect if the mouse hover an object
					if (tempSlot->DetectClick(mousePos, tempSlot))
					{
						if (inputs->IsLongPress("Attack"))
						{
							onDrag = true;
							itemDragged = GetItemByGUI(index);
							break;
						}
						hoveringSomething = true;
						if (hoveredUI != nullptr)
						{
							// Detect if the hovered object is the same as the last one
							if (hoveredUI == tempSlot)
							{
								break;
							}
							informations->SetEnabled(false);
						}
						// Change the hovered object, set the timer at 0
						informationsPicked = false;
						hoverTimer = 0;
						hoveredUI = tempSlot;
						break;
					}
					index++;
				}
				if (hoveringSomething == true || onDrag == true) break;
			}
			hoverTimer += GameData::DeltaTime;
			// Hovering informations loader
			if (hoverTimer > SHOW_INFO_TIMER && informationsPicked == true && hoveringSomething == false)
			{
				informations->SetEnabled(false);
				informationsPicked = false;
				hoveredUI = nullptr;
			}
			else if (hoverTimer > SHOW_INFO_TIMER && hoveredUI != nullptr)
			{
				//check the place between hovered gui bounds and inventory bounds to place the informations accordingly

				sf::Vector2f topLeft;
				sf::Vector2f bottomRight;

				sf::Vector2f hoveredTopLeft = hoveredUI->GetTopLeft();
				sf::Vector2f hoveredBottomRight = hoveredUI->GetBottomRight();

				// if the place at the left side of the hovered ui is upper than the right side
				if (hoveredTopLeft.x > 1.0f - hoveredBottomRight.x)
				{
					bottomRight.x = hoveredUI->GetTopLeft().x;
					topLeft.x = bottomRight.x - INFO_SIZE.x;
				}
				else
				{
					topLeft.x = hoveredUI->GetBottomRight().x;
					bottomRight.x = topLeft.x + INFO_SIZE.x;
				}

				// if the place at the up side of the hovered ui is upper than the down side
				if (hoveredTopLeft.y > 1.0f - hoveredBottomRight.y)
				{
					bottomRight.y = hoveredUI->GetBottomRight().y;
					topLeft.y = bottomRight.y - INFO_SIZE.y;
				}
				else
				{
					topLeft.y = hoveredUI->GetTopLeft().y;
					bottomRight.y = topLeft.y + INFO_SIZE.y;
				}

				informations->SetNormalizedValue(GUI_LEFT, topLeft.x);
				informations->SetNormalizedValue(GUI_TOP, topLeft.y);
				informations->SetNormalizedValue(GUI_RIGHT, bottomRight.x);
				informations->SetNormalizedValue(GUI_BOTTOM, bottomRight.y);

				GUI* area = guiMan.GetNode(gui->GetChildren()[0]);
				GUIMaskingContainer* guiMaskingContainer = (GUIMaskingContainer*)guiMan.GetNode(area->GetChildren()[0]);

				informations->SetCornerPosition(sf::Vector2f(informations->GetCornerPosition(TOP_LEFT).x, informations->GetCornerPosition(TOP_LEFT).y - guiMaskingContainer->GetOffset().y), TOP_LEFT);
				informations->SetCornerPosition(sf::Vector2f(informations->GetCornerPosition(BOTTOM_RIGHT).x, informations->GetCornerPosition(BOTTOM_RIGHT).y - guiMaskingContainer->GetOffset().y), BOTTOM_RIGHT);
				
				if (informationsPicked == false)
				{
					//Get object info
					Item* itemInfo = nullptr;
					int position = 0;
					for (auto i = inventory.begin(); i != inventory.end(); i++)
					{
						if (itemInfo != nullptr) break;
						for (int j = 0; j < i->second; j++)
						{
							if (position == index)
							{
								itemInfo = i->first;
								break;
							}
							position++;
						}
					}
					if (itemInfo == nullptr)
					{
						hoveredUI = nullptr;
						informations->SetEnabled(false);
						informationsPicked = false;
						return;
					}

					//Set informations
					GUIText* name = (GUIText*)guiMan.GetNode(informations->GetChildren()[0]);
					name->SetText(itemInfo->name);

					GUIText* description = (GUIText*)guiMan.GetNode(informations->GetChildren()[1]);
					description->SetText("Description : \n" + itemInfo->description);
					description->SetText(TextEmplacer(description->GetSfText(), description->GetSfShape(), description->GetText()));

					GUIText* rarity = (GUIText*)guiMan.GetNode(informations->GetChildren()[2]);
					rarity->SetText("Rarity :\n" + GetTextRarity(itemInfo->rarity));

					GUIText* weight = (GUIText*)guiMan.GetNode(informations->GetChildren()[4]);
					std::string weightText = std::to_string(itemInfo->weight);
					weightText = weightText.substr(0, weightText.find(".") + 3);
					weight->SetText("Weight :\n" + weightText);

					GUIImage* image = (GUIImage*)guiMan.GetNode(informations->GetChildren()[5]);
					image->ChangeTexture(*itemInfo->_sprite->getTexture());

					informationsPicked = true;
					informations->SetEnabled(true);
				}
			}
		}
		else 
		{
			//Drag released 
			if (inputs->IsLongRelease("Attack"))
			{
				if (!gui->DetectClick((sf::Vector2i)GameData::GetMousePos(), gui)) 
				{
					if (RemoveItem(itemDragged, 1)){}

				}
				onDrag = false;
				itemDragged = nullptr;
			}
		}
	}
	else 
	{
		gui->SetEnabled(false);
		itemDragged = nullptr;
		onDrag = false;
		informationsPicked = false;
		hoveredUI = nullptr;
	}
}

// Removing item
bool PlayerInventory::RemoveItem(Item* _item, unsigned int _quantity)
{
	if (_quantity != 0)
	{
		auto itr = inventory.find(_item);
		if (itr != inventory.end())
		{
			if ((itr->second - _quantity) >= 0)
			{
				itr->second -= _quantity;
				if(itr->second == 0) inventory.erase(itr->first);
				return true;
			}
		}
	}
	return false;
}

float PlayerInventory::GetTotalWeight()
{
	float totalWeight = 0.0f;

	for (auto i = inventory.begin(); i != inventory.end(); i++) 
	{
		totalWeight += i->first->weight * i->second;
	}

	return totalWeight;
}

void PlayerInventory::ShowInventory(sf::RenderWindow& _window)
{
	if (showingInventory == true)
	{
		gui->SetEnabled(true);
		GameData::CharaCtrl->GetGrapHook()->SetShowTarget(false);
		GUIManager& guiMan = *GUIManager::GetInstance(); //Get gui
		ScrollManager& scrollMan = *ScrollManager::GetInstance();

		GUIGridLayout* layout = ScrollManager::GetInstance()->GetStruct("PlayerInventory")->layout;

		int index = 0;
		for (auto i = inventory.begin(); i != inventory.end(); i++)
		{
			for (int j = 0; j < i->second; j++)
			{
				GUIImage* tempSlot = (GUIImage*)guiMan.GetNode(layout->GetChildren()[index]);
				GUIImage* itemSprite = (GUIImage*)guiMan.GetNode(tempSlot->GetChildren()[0]);

				itemSprite->SetEnabled(true);
				itemSprite->ChangeTexture(*i->first->_slotSprite->getTexture());

				index++;

				if (index >= layout->GetChildren().size()) scrollMan.AddRow("PlayerInventory", "ConfigFiles/GUI.dat/invSlot.dat");
			}
		}
		for (; index < layout->GetChildren().size(); index++)
		{
			GUIImage* tempSlot = (GUIImage*)guiMan.GetNode(layout->GetChildren()[index]);
			GUIImage* itemSprite = (GUIImage*)guiMan.GetNode(tempSlot->GetChildren()[0]);

			itemSprite->SetEnabled(false);
		}
		if (itemDragged != nullptr) 
		{
			BlitSprite(*itemDragged->_slotSprite, GameData::GetRelativeMousePos(), 0, _window);
		}
	}
	else
	{
		GameData::CharaCtrl->GetGrapHook()->SetShowTarget(true);
		gui->SetEnabled(false);
	}
}

void PlayerInventory::Save(std::string _path)
{
	nlohmann::json playerInvData;
	for (auto i = inventory.begin(); i != inventory.end(); i++)
	{
		std::string itemName = i->first->name; 
		playerInvData[itemName] = i->second;
	}
	std::fstream file(_path, std::ios::out | std::ios::trunc);
	if (file.is_open()) 
	{
		file << playerInvData;
		file.close();
	}
}

void PlayerInventory::Load(std::string _path)
{
	nlohmann::json playerInvData;
	std::fstream file(_path, std::ios::in);
	if (file.is_open())
	{
		file >> playerInvData;
		file.close();
		inventory.clear();
		for (auto i = playerInvData.begin(); i != playerInvData.end(); i++)
		{
			inventory.insert({ItemManager::GetItem(i.key()), i.value()});
		}
	}
}

Item* PlayerInventory::GetItemByGUI(int index)
{
	int currentIndex = 0;

	for (auto i = inventory.begin(); i != inventory.end(); i++)
	{
		for (int j = 0; j < i->second; j++)
		{
			if (currentIndex == index)
			{
				return i->first;
			}
			currentIndex++;
		}
	}
	return nullptr;
}