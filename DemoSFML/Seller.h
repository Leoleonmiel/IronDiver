#ifndef SELLER_H
#define SELLER_H

#include "Guest.h"

struct Item;

class Seller : public Guest
{
public:
    Seller(Item* _item, sf::Vector2f _startPos)
    {
        itemToSell = _item;
        pos = _startPos;
    }

    Seller() {};

    ~Seller() {};

    void Trade() override;
    bool CanTrade() override;

private:
    std::string name;
    std::string sentence;
    Item* itemToSell = nullptr;
    int quantity = 0;
    sf::Sprite* sprite = nullptr;
    sf::Vector2f pos;
    int walkBoost = 0;
    int damageBoost = 0;
    bool unlockSpell = false;
};

#endif // SELLER_H
