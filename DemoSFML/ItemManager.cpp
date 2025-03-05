#include "ItemManager.h"
#include "TextureManager.h"
#include "Item.h"
#include <iostream>

std::unordered_map<size_t, Item*> ItemManager::items;
std::hash<std::string> ItemManager::hasher;
nlohmann::json ItemManager::itemData;
std::string ItemManager::path;

// Read the json file to create all items accordate to the data
void ItemManager::Init(std::string _jsonPath)
{
	path = _jsonPath;
	std::ifstream file(path);
	if (file.is_open())
	{
		file >> itemData;
	}
	else 
	{
		std::cout << "Error: ItemManager::Init() - Cannot open file: " << path << std::endl;
	}
	file.close();
	for (auto i = itemData.begin(); i != itemData.end(); i++)
	{
		size_t hashValue = hasher(i.key());
		if (items.find(hashValue) != items.end()) continue;
		Item* newItem = new Item();
		newItem->name = i.key();
		for (auto j = i->begin(); j != i->end(); j++)
		{
			if (j.key() == "Price")
			{
				newItem->price = j.value();
			}
			else if (j.key() == "Id")
			{
				newItem->id = hashValue;
			}
			else if (j.key() == "Weight")
			{
				newItem->weight = j.value();
			}
			else if (j.key() == "Rarity")
			{
				char value = (int)j.value();
				newItem->rarity = Rarity(value);
			}
			else if (j.key() == "IsUsable")
			{
				newItem->isUsable = j.value();
			}
			else if (j.key() == "Description")
			{
				newItem->description = j.value();
			}
			else if (j.key() == "Crafts")
			{
				for (auto k = j->begin(); k != j->end(); k++)
				{
					std::pair<std::string, unsigned int> pair;
					pair.first = k.key();
					pair.second = k.value();
					newItem->crafts.push_back(pair);
				}
			}
			else if (j.key() == "ImagePath")
			{
				if(j.value() != "None") newItem->_sprite = TextureManager::CreateSprite(j.value(), true);
			}
			else if (j.key() == "ImageSlotPath")
			{
				if (j.value() != "None") newItem->_slotSprite = TextureManager::CreateSprite(j.value(), true);
			}
		}
		items.insert({ hashValue, newItem });
	}
}

void ItemManager::SaveData()
{
	std::ofstream file(path, std::ios::trunc);
	if (file.is_open())
	{
		file << itemData;
	}
	file.close();
	std::ifstream readFile(path);
	if (readFile.is_open())
	{
		readFile >> itemData;
	}
	readFile.close();
}

Item* ItemManager::GetItem(std::string _name)
{
	size_t hashValue = hasher(_name);
	return items.at(hashValue);
}