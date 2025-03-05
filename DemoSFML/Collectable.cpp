#include "Collectable.h"
#include "common.h"
#include "Item.h"
#include "Inventory.h"
#include "CreateState.h"

void Collectable::Display(sf::RenderWindow& _window)
{
    BlitSprite(*m_sprite, m_pos, 0, _window);
}

Collectable::Collectable(const EntityDef& _def, Item* _item, sf::Vector2f _pos) : Entity(_def, _pos)
{
    m_item = _item;
    m_pos = _pos;
    m_sprite = _item->_sprite;
    m_state = new CreateState(this); 
}

void Collectable::Update(sf::Vector2f _characterPos, PlayerInventory* _inventory)
{
    m_state->Update();
}

void Collectable::AddToInventory(PlayerInventory* _inventory)
{
    _inventory->AddItem(m_item, 1);
    m_isInInventory = true;
}





