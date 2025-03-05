#ifndef COLLECTABLE_H
#define COLLECTABLE_H

//#include "Inventory.h"
#include "Entity.h"
#include <SFML/Graphics.hpp>

class PlayerInventory;
struct EntityDef;
struct Item;

class Collectable : Entity
{
public:
    Collectable(const EntityDef& _def, Item* _item, sf::Vector2f _pos);


    ~Collectable() {}

#pragma region Getter

    Item* GetItem()
    {
        return m_item;
    }

    sf::Vector2f GetPos()
    {
        return m_pos;
    }

    sf::Sprite* GetSprite()
    {
        return m_sprite;
    }

    bool GetIsInInventory()
    {
        return m_isInInventory;
    }

    float GetTimeElapsed()
    {
        return m_timeElapsed;
    }
#pragma endregion

    void SetTimeElapsed(float _value)
    {
        m_timeElapsed = _value;
    }

    void SetPosition(sf::Vector2f _value)
    {
        m_pos = _value;
    }

    void SetIsInInventory(bool _value)
    {
        m_isInInventory = _value;
    }

#pragma region Function

    void Update(sf::Vector2f _characterPos, PlayerInventory* _inventory);
    void Display(sf::RenderWindow& _window);
    void AddToInventory(PlayerInventory* _playerInventory);
#pragma endregion

private:
    Item* m_item = nullptr;
    sf::Vector2f m_startPos;
    sf::Vector2f m_pos;
    sf::Sprite* m_sprite = nullptr;
    float m_timeElapsed = 0;
    bool m_isInInventory = false;
};

#endif // COLLECTABLE_H
