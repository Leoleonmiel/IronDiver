#ifndef CRAFTMANAGER_H
#define CRAFTMANAGER_H

#include <string>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

struct Item;
class GUI;

class CraftManager
{
public:
	~CraftManager();
	void Init();
	static CraftManager* GetInstance();
	/// <summary>
	/// Return -1 if the item is not a craftable item
	/// </summary>
	int IsCraftable(Item* _item);
	void Craft(std::string, int = 1);
	void Update();
	void Display(sf::RenderWindow &_window);
	static void FreeMemory();
private:
	static CraftManager* Instance;
	GUI* craft = nullptr;
	std::vector<GUI*> buttons;
	std::vector<Item*> craftableItems;
	GUI* selectedCraft = nullptr;
	GUI* craftButton = nullptr;
};

#endif // !CRAFTMANAGER_H


