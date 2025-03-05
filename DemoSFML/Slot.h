#ifndef SLOT_H
#define SLOT_H

#include "common.h"
#include "item.h"

enum State
{
    VISIBLE,
    HIDDEN
};

class Slot
{
public:
    Slot(std::string _spritePath, sf::Vector2f _position, Item* _item)
    {
        m_sprite = new sf::Sprite(LoadSprite(_spritePath, false));
        m_state = VISIBLE;
        m_position = _position;
        m_itemNeeded = _item;
    }

    Slot() {};

    ~Slot() {};

    void Flashing(float);
    void Update(float);
    void Display(sf::RenderWindow&);

    bool PointInsideSprite(sf::Vector2f point)
    {
        sf::FloatRect bounds = m_sprite->getGlobalBounds();
        return bounds.contains(point);
    }

    sf::Vector2f GetPos()
    {
        return m_position;
    }

    sf::Sprite GetSprite()
    {
        return *m_sprite;
    }

    void SetItem(Item* _value)
    {
        m_item = _value;
    }

    void SetFlashing(bool _value)
    {
        flashing = _value;
    }

    void SetItemNeeded(Item* _value)
    {
        m_itemNeeded = _value;
    }
    Item* GetItem()
    {
        return m_item;
    }
    Item* GetItemNeeded()
    {
        return m_itemNeeded;
    }

private:
    sf::Sprite* m_sprite = nullptr;
    sf::Vector2f m_position;
    float m_elapsedTime = 0.f;
    State m_state = (State) 0;
    bool canChangeState = false;
    Item* m_item = nullptr;
    Item* m_itemNeeded = nullptr;
    bool flashing = false;

};

#endif // SLOT_H
