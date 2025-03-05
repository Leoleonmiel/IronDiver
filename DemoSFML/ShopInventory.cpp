#include "Inventory.h"
#include "ItemManager.h"
#include "Item.h"
#include "TextureManager.h"
#include "common.h"
#include "GUI.h"
#include "GUIManager.h"
#include "ScrollManager.h"
#include "InputManager.h"
#include "Slot.h"

std::unordered_map<size_t, InventoryItem*> ShopInventory::inventory;
std::hash<std::string> ShopInventory::hasher;
int ShopInventory::m_gold = 500;


//init GUI elements
void ShopInventory::InitInventoryDisplay() 
{
	GUIManager& gui = *GUIManager::GetInstance(); //get gui
	ScrollManager& scrollMan = *ScrollManager::GetInstance();

	GUIImage::TextureSearchFunction = &TextureManager::GetTexture;

	GUIGridLayout* layout = (GUIGridLayout*)gui.GetNodeByTag("invLayout");
	GUIMaskingContainer* parent = (GUIMaskingContainer*)gui.GetNode(layout->GetParent());
	layout->SetRowCount(layout->GetRowCount() + int(inventory.size() / (layout->GetColumnCount() * layout->GetRowCount())));

	scrollMan.CreateNewStruct(parent, layout, "ShopInventory");
	for (int i = 0 ; i < layout->GetRowCount(); i++)
	{
		scrollMan.AddRow("ShopInventory", "ConfigFiles/GUI.dat/invSlot.dat");
	}

}

void ShopInventory::AddItem(Item* _item, unsigned int _amount)
{
	size_t value = hasher(_item->name);
	
	// Check if _item is already in inventory
	if (Contains(_item->name))
	{
		inventory.at(value)->amount += _amount;
	}
	else 
	{
		InventoryItem* tempInventoryItem = new InventoryItem();
		tempInventoryItem->amount = _amount;
		tempInventoryItem->item = _item;

		inventory.insert({ value, tempInventoryItem });

		// Adapt inventory display if necessary
		ScrollManager& scrollMan = *ScrollManager::GetInstance();
		ScrollManager::ScrollStruct* scrollStruct = scrollMan.GetStruct("ShopInventory");
		if (scrollStruct != nullptr) 
		{
			if (inventory.size() > (scrollStruct->layout->GetColumnCount() * scrollStruct->layout->GetRowCount())) scrollMan.AddRow("ShopInventory", "ConfigFiles/GUI.dat/invSlot.dat");
		}
	}
}

// add all player inventory items to shop inventory
void ShopInventory::AddPlayerInventoryItems(PlayerInventory& _playerInventory)
{
	auto map = _playerInventory.GetInventory();
	for (auto i = map.begin(); i != map.end(); i++)
	{
		AddItem(i->first, i->second);
	}
}

bool ShopInventory::RemoveItem(Item* _item, unsigned int quantity)
{
	std::string _itemName = _item->name;
	size_t value = hasher(_itemName);

	// Check if _item exist in inventory
	bool useIsPossible = Contains(_itemName, quantity);

	if (useIsPossible)
	{
		inventory.at(value)->amount -= quantity;
		if (inventory.at(value)->amount <= 0)
		{
			inventory.erase(value);
		}
	}
	return useIsPossible;
}

void ShopInventory::ShowInventory(sf::RenderWindow& _window, Slot* _slot)
{
	GUIManager& gui = *GUIManager::GetInstance(); //get gui

	GUIGridLayout* layout = ScrollManager::GetInstance()->GetStruct("ShopInventory")->layout;

	int index = 0;
	// Show used slots
	for (auto i = inventory.begin(); i != inventory.end(); i++)
	{
		GUI* tempSpriteBG = gui.GetNode(layout->GetChildren()[index]);

		GUIImage* tempSprite = (GUIImage*)gui.GetNode(tempSpriteBG->GetChildren()[0]);
		tempSprite->ChangeTexture(*i->second->item->_slotSprite->getTexture());
		GUIText* amount = (GUIText*)gui.GetNode(tempSpriteBG->GetChildren()[1]);

		amount->SetText(std::to_string(i->second->amount));
		tempSprite->SetEnabled(true);
		amount->SetEnabled(true);
		index++;
	}

	// Hide unused slots
	for (; index < layout->GetChildren().size(); index++)
	{
		GUI* tempSpriteBG = gui.GetNode(layout->GetChildren()[index]);

		GUIImage* tempSprite = (GUIImage*)gui.GetNode(tempSpriteBG->GetChildren()[0]);
		tempSprite->SetEnabled(false);

		GUIText* amount = (GUIText*)gui.GetNode(tempSpriteBG->GetChildren()[1]);
		amount->SetEnabled(false);
	}
	GUIText* gold = (GUIText*)gui.GetNodeByTag("goldArea");
	gold->SetText(std::to_string(m_gold) + "$");

	if (m_itemSprite != nullptr)
	{
		BlitSprite(*m_itemSprite, sf::Vector2f(GameData::GetMousePos().x - 40, GameData::GetMousePos().y - 25), 0, _window);
	}
}

bool ShopInventory::Contains(std::string _itemName, int _amount)
{
	size_t value = hasher(_itemName);

	//Item does not exist in inventory
	if (inventory.find(value) == inventory.end())
	{
		return false;
	}
	else
	{
		if (inventory.at(value)->amount >= _amount)
		{
			return true;
		}
		return false;
	}
}

void ShopInventory::Save(std::string _path)
{
	nlohmann::json shopInvData;
	for (auto i = inventory.begin(); i != inventory.end(); i++)
	{
		std::string itemName = i->second->item->name;
		shopInvData[itemName] = i->second->amount;
	}
	shopInvData["golds"] = m_gold;
	std::fstream file(_path, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		file << shopInvData;
		file.close();
	}
}

void ShopInventory::Load(std::string _path)
{
	nlohmann::json shopInvData;
	std::fstream file(_path, std::ios::in);
	if (file.is_open())
	{
		file >> shopInvData;
		file.close(); 
		inventory.clear();
		for (auto i = shopInvData.begin(); i != shopInvData.end(); i++)
		{
			if (i.key() == "golds") 
			{
				m_gold = i.value();
				continue;
			}
			size_t value = hasher(i.key());
			InventoryItem* tempItem = new InventoryItem();
			tempItem->amount = i.value();
			tempItem->item = ItemManager::GetItem(i.key());
			inventory.insert({ value, tempItem });
		}
	}
}
void ShopInventory::Update(Slot* _slot, Item* _item)
{
	if (showingInventory)
	{
		sf::Vector2i mousePos = (sf::Vector2i)GameData::GetMousePos();

		GUIManager& guiMan = *GUIManager::GetInstance(); //Get gui

		GUIGridLayout* layout = ScrollManager::GetInstance()->GetStruct("ShopInventory")->layout;
		bool hoveringSomething = false;
		int index = 0;
		auto inputs = InputManager::GetInstance();

	//	std::cout << itemIndex << std::endl;
		for (; index < inventory.size(); index++)
		{
			
			GUI* tempSlot = guiMan.GetNode(layout->GetChildren()[index]);
			//Detect if the mouse hover an object
			if (tempSlot->DetectClick(mousePos, tempSlot) && !onDrag && _slot->GetItem() == nullptr)
			{
				//	std::cout << "Index cliqué : " << itemIndex << std::endl;
				itemIndex = index;
				
				if (inputs->IsLongPress("Attack") && m_itemSprite == nullptr)
				{
					onDrag = true;
					itemGUIIMage = (GUIImage*)guiMan.GetNode(tempSlot->GetChildren()[0]);
					itemAmountText = (GUIText*)guiMan.GetNode(tempSlot->GetChildren()[1]);
					itemGUIIMage->SetColorAlpha(0);
					if (itemGUIIMage != nullptr)
					{
						// tempSlot est un GUIImage
						m_itemSprite = new sf::Sprite(*itemGUIIMage->GetTexture());

						//std::cout << tempImage->GetTexture() << std::endl;
					}
					else
					{
						// tempSlot n'est pas un GUIImage
						std::cout << "tempSlot n'est pas un GUIImage." << std::endl;
					}
					break;
				}


			}
			if (itemIndex != -1)
			{
				if (inputs->IsLongRelease("Attack") && _slot->GetItem() == nullptr)
				{
					if (m_itemSprite != nullptr)
					{
						if (_slot->PointInsideSprite(GameData::GetMousePos()))
						{
							if (_slot->GetItemNeeded() == GetItemByGUI(itemIndex))
							{
								_slot->SetItem(GetItemByGUI(itemIndex));
							}
							if (itemAmountText->GetText() != "0")
							{
								itemGUIIMage->SetColorAlpha(255);
							}
						/*	if (itemIndex >= 0 && itemIndex < inventory.size()) {
								inventory.at(itemIndex)->amount = 0;
							}*/
						}
						else
						{
							itemGUIIMage->SetColorAlpha(255);
						}
					}

					onDrag = false;
					delete m_itemSprite;
					m_itemSprite = nullptr;
				}
			}


			if (onDrag)
			{  
				/*if (_slot->GetItem() != nullptr)
				{*/
					if (_slot->GetItemNeeded() == GetItemByGUI(itemIndex))
					{
						_slot->Flashing(GameData::DeltaTime);
					}			
			}
			else
			{
				_slot->SetFlashing(false);
			}
		}
	
	
	}
}
Item* ShopInventory::GetItemByGUI(int index)
{
    int currentIndex = 0; 

    for (auto i = inventory.begin(); i != inventory.end(); i++)
    {
        if (currentIndex == index) 
        {
            return i->second->item; 
        }
        currentIndex++; 
    }

    return nullptr; 
}