#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <sfml/Graphics.hpp>

enum Rarity 
{
    COMMON,
    RARE,
    EPIC,
    MYTHIC,
    LEGENDARY
};

struct Item 
{
    ~Item() { delete _sprite; }

    bool isUsable = false;
    size_t id = 0;
    int price = 0;
    float weight = 0.f;
    Rarity rarity = COMMON;
    std::string description;
    std::string name;
    sf::Sprite* _sprite = nullptr;
    sf::Sprite* _slotSprite = nullptr;
    std::vector<std::pair<std::string, unsigned int>> crafts;
    //to fix an error cause the custom type for std::map

    bool operator<(const Item& other) const 
    {
        return id < other.id;
    }
};

struct Consumable : Item 
{
    int value;
};

struct InventoryItem 
{
    ~InventoryItem() { delete item; }
    Item* item = nullptr;
    int amount;
};


#endif