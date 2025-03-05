#ifndef INVENTORY_H
#define INVENTORY_H

#include <sfml/Graphics.hpp>
#include <unordered_map>
#include <map>
#include "nlohmann/json.hpp"
#include "Slot.h"

class GUIImage;
class GUIText;
class GUI;
class ShopInventory;
class PlayerInventory;
struct Item;
struct InventoryItem;

class Inventory 
{
public:
	virtual void AddItem(Item* _item, unsigned int quantity = 1) = 0;
	virtual bool RemoveItem(Item* _item, unsigned int quantity = 1) = 0;
	//virtual void ShowInventory(sf::RenderWindow& _window) = 0;
	virtual void Save(std::string _path) = 0;
	virtual void Load(std::string _path) = 0;
	static ShopInventory* shopInventory;
	static PlayerInventory* playerInventory;
};

class PlayerInventory : Inventory 
{

private:
	std::map<Item*, int> inventory;
	GUI* gui = nullptr;
	float hoverTimer = 0;
	GUI* hoveredUI = nullptr;
	GUI* informations = nullptr;
	bool informationsPicked = false;
	bool onDrag = false;
	Item* itemDragged = nullptr;
public:
	PlayerInventory();
	~PlayerInventory() {}

	void AddItem(Item* _item, unsigned int quantity = 1);
	void ShowInventory(sf::RenderWindow& _window);
	Item* GetItemByGUI(int index);
	void Update();
	void Save(std::string _path);
	void Load(std::string _path);
	bool RemoveItem(Item* _item, unsigned int quantity = 1);
	float GetTotalWeight();
	std::map<Item*, int> GetInventory(){return inventory;}

	bool showingInventory = false;
};

class ShopInventory : Inventory
{
public:
	ShopInventory() = default;
	~ShopInventory() = default;

	bool Contains(std::string _itemName, int _amount = 1);
	void AddItem(Item* _item, unsigned int quantity = 1);
	void AddPlayerInventoryItems(PlayerInventory& _playerInventory);
	void InitInventoryDisplay();
	
	int GetGold() { return m_gold; }
	void AddGold(int _value) { m_gold += _value; }
	void RemoveGold(int _value) { m_gold -= _value; }
	/// <summary>
	/// Return true if the amount gived isn't bigger than the amount in stock
	/// </summary>
	bool RemoveItem(Item* _item, unsigned int quantity = 1);
	void ShowInventory(sf::RenderWindow& _windo, Slot*);
	void Save(std::string _path);
	void Load(std::string _path);
	void Update(Slot*, Item*);
	Item* GetItemByGUI(int index);

	bool showingInventory = false;
private:
	static int m_gold;
	static std::unordered_map<size_t, InventoryItem*> inventory;
	static std::hash<std::string> hasher;
	sf::Sprite* m_itemSprite;
	int itemIndex = -1;
	bool onDrag = false;
	GUIImage* itemGUIIMage = nullptr;
	GUIText* itemAmountText = nullptr;
	
};

#endif

