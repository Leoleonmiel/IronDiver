#ifndef BUYER_H
#define BUYER_H

#include "Guest.h"

struct Item;

class Buyer : public Guest
{
public:
    Buyer(Item* _item, sf::Vector2f _startPos)
    {
        itemToBuy = _item;
        pos = _startPos;
    }

    Buyer() {};

    ~Buyer() {};
    void Trade() override;
    bool CanTrade() override;

private:
    std::string name;
    std::string sentence;
    Item* itemToBuy = nullptr;
    int quantity = 0;
    sf::Sprite* sprite = nullptr;
    sf::Vector2f pos;
};

#endif // BUYER_H
