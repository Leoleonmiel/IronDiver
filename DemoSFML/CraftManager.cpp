#include "CraftManager.h"
#include "ItemManager.h"
#include "Item.h"
#include "Inventory.h"
#include "TextureManager.h"
#include "common.h"
#include "InputManager.h"
#include "GUIManager.h"
#include "GUI.h"
#include "SoundManager.h"
#include "ScrollManager.h"

CraftManager* CraftManager::Instance = nullptr;

CraftManager::~CraftManager()
{
	for (int i = (int)craftableItems.size() - 1; i > 0; i--)
	{
		craftableItems.erase(craftableItems.begin() + i);
		craftableItems.shrink_to_fit();
	}
}

void CraftManager::Init()
{
	//Load craftable items
	auto map = ItemManager::GetItemsMap();
	for (auto i = map.begin(); i != map.end(); i++)
	{
		if (IsCraftable(i->second) == -1)
		{
			continue;
		}
		craftableItems.push_back(ItemManager::GetItem(i->second->name));
	}

	GUIManager& gui = *GUIManager::GetInstance(); //get gui
	ScrollManager& scroll = *ScrollManager::GetInstance();

	// Load UI data
	std::ifstream slotStream("ConfigFiles/GUI.dat/itemSlot.dat");
	std::ifstream craftStream("ConfigFiles/GUI.dat/craftBackground.dat");

	GUIImage::TextureSearchFunction = &TextureManager::GetTexture;
	if (craftStream.is_open())
	{
		gui.LoadPrefab(craftStream, gui.GetID(gui.GetNodeByTag("CraftContainer")) + 1);
		craftStream.close();
	}

	//Load craft UI
	craft = (GUIImage*)gui.GetNodeByTag("craftBackground");

	GUI* craftArea = gui.GetNode(craft->GetChildren()[0]);

	GUIMaskingContainer* guiMaskingContainer = (GUIMaskingContainer*)gui.GetNode(craftArea->GetChildren()[0]);
	GUIGridLayout* guiBackgroundLayout = (GUIGridLayout*)gui.GetNode(guiMaskingContainer->GetChildren()[0]);
	
	scroll.CreateNewStruct(guiMaskingContainer, guiBackgroundLayout, "Craft");

	craftButton = gui.GetNode(craftArea->GetChildren()[1]);

	for (int i = 0; i < craftableItems.size(); i++)
	{
		scroll.AddRow("Craft", "ConfigFiles/GUI.dat/craftSign.dat");

		GUIImage* tempSign = (GUIImage*)gui.GetNode(guiBackgroundLayout->GetChildren()[i]);

		GUIText* itemName = (GUIText*)gui.GetNode(tempSign->GetChildren()[1]);
		GUIGridLayout* itemSlotLayout = (GUIGridLayout*)gui.GetNode(tempSign->GetChildren()[0]);

		GUI* spriteBG = gui.GetNode(tempSign->GetChildren()[2]);
		GUIImage* craftedItemSprite = (GUIImage*)gui.GetNode(spriteBG->GetChildren()[0]);
		craftedItemSprite->ChangeTexture(*craftableItems[i]->_slotSprite->getTexture());

		itemName->SetText(craftableItems[i]->name);

		//Add itemSlots
		for (int j = 0; j < craftableItems[i]->crafts.size(); j++)
		{
			if (slotStream.is_open())
			{
				gui.LoadPrefab(slotStream, itemSlotLayout->GetID() + 1);
				slotStream.seekg(0);
			}

			GUI* tempSlot = (GUI*)gui.GetNode(itemSlotLayout->GetChildren()[j]);

			GUIText* slotItemAmount = (GUIText*)gui.GetNode(tempSlot->GetChildren()[2]);
			slotItemAmount->SetText(std::to_string(craftableItems[i]->crafts[j].second));

			GUIImage* slotItemImage = (GUIImage*)gui.GetNode(tempSlot->GetChildren()[1]);
			Item* tempItem = ItemManager::GetItem(craftableItems[i]->crafts[j].first);
			if(tempItem->_sprite != nullptr)slotItemImage->ChangeTexture(*tempItem->_sprite->getTexture());
			else slotItemImage->ChangeTexture(*craftableItems[i]->_slotSprite->getTexture());
		}
		buttons.push_back(spriteBG);
	}
	slotStream.close();
	craft->SetEnabled(false);
}

CraftManager* CraftManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new CraftManager();
	}
	return Instance;
}

int CraftManager::IsCraftable(Item* _item)
{
	if (_item->crafts.size() > 0)
	{
		for (auto j = _item->crafts.begin(); j != _item->crafts.end(); j++)
		{
			//If he don't have all the items needed, is not craftable
			if (!Inventory::shopInventory->Contains(j->first, j->second))
			{
				return false;
			}
		}
		return true;
	}
	return -1;

}

void CraftManager::Update()
{
	craft->SetEnabled(Inventory::shopInventory->showingInventory);
	if (craft->IsEnabled())
	{
		auto inputs = InputManager::GetInstance();
		if (inputs->IsSinglePress("Attack"))
		{
			for (int i = 0; i < buttons.size(); i++)
			{
				// Select an item to craft
				if (buttons[i]->DetectClick((sf::Vector2i)GameData::GetMousePos(), buttons[i]))
				{
					if (IsCraftable(craftableItems[i]) != 1) continue;
					if (selectedCraft == buttons[i])
					{
						selectedCraft = nullptr;
					}
					else 
					{
						selectedCraft = buttons[i];
					}
					SoundManager::GetInstance()->Play("TESTUI");
				}
			}
			// Craft button pressed
			if (craftButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), craftButton) && selectedCraft != nullptr) 
			{
				for (int i = 0; i < buttons.size(); i++)
				{
					if (selectedCraft == buttons[i]) 
					{
						SoundManager::GetInstance()->Play("Craft");
						Craft(craftableItems[i]->name);
						if (IsCraftable(craftableItems[i]) != 1) selectedCraft = nullptr;
						break;
					}
				}
			}
		}
	}
	

}

void CraftManager::Display(sf::RenderWindow& _window)
{
	GUIManager& gui = *GUIManager::GetInstance(); //Get gui
	ScrollManager& scroll = *ScrollManager::GetInstance();

	ScrollManager::ScrollStruct* scrollStruct = scroll.GetStruct("Craft");

	for (int i = 0; i < craftableItems.size(); i++)
	{
		GUIImage* tempSign = (GUIImage*)gui.GetNode(scrollStruct->layout->GetChildren()[i]);
		GUIImage* tempSprite = (GUIImage*)gui.GetNode(tempSign->GetChildren()[2]);

		// make the selected item white
		if(selectedCraft != nullptr)
		{
			if (selectedCraft == tempSprite) 
			{
				tempSprite->SetColor(sf::Color(255, 255, 255));
				continue;
			}
		}

		// make the uncraftable items gray
		int value = IsCraftable(craftableItems[i]);
		if (value == 0) tempSprite->SetColor(sf::Color(128, 128, 128));
		else if (value > 0) tempSprite->SetColor(sf::Color(200, 200, 200));
		else continue;
	}
	craft->Display(_window);
}

void CraftManager::FreeMemory()
{
	delete Instance;
	Instance = nullptr;
}

void CraftManager::Craft(std::string _itemName, int _amount)
{
	Item* craftedItem = ItemManager::GetItem(_itemName);

	for (auto i = craftedItem->crafts.begin(); i != craftedItem->crafts.end(); i++)
	{
		Item* materialItem = ItemManager::GetItem(i->first);
		if (!Inventory::shopInventory->RemoveItem(materialItem, i->second)) 
		{
			return;
		}
	}

	Inventory::shopInventory->AddItem(craftedItem, _amount);
}
