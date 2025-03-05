#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include <unordered_map>
#include "nlohmann/json.hpp"
#include <fstream>
#include <unordered_map>

struct Item;

class ItemManager
{
public:
	static void Init(std::string);
	static void SaveData();
	static Item* GetItem(std::string);
	static std::unordered_map<size_t, Item*> GetItemsMap() { return items; }
private:
	static std::unordered_map<size_t,Item*> items;
	static nlohmann::json itemData;
	static std::string path;
	static std::hash<std::string> hasher;
};

#endif // !ITEMMANAGER_H


