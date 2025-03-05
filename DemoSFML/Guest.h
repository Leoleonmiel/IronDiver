#ifndef GUEST_H
#define GUEST_H

#include "Item.h"
#include "common.h"
#include "Inventory.h"
#include "DialogModule.h"
struct Item;

class Guest
{
public:

    enum State
    {
        WAIT,
        WALK_TO_SHOP,
        WALK_TO_EXIT,
        BUSY,
        READY_TO_INTERACT,
        READY_TO_LEAVE,
    };

#pragma region Constructor

    Guest() = default;

    ~Guest();

#pragma endregion

#pragma region Getter

    std::string GetName()
    {
        return m_name;
    }

    std::string GetSentence()
    {
        return m_sentence;
    }

    std::string GetThanksSentence()
    {
        return m_thanksSentence;
    }

    std::string GetDenySentence()
    {
        return m_denySentence;
    }

    Item* GetItem()
    {
        return m_item;
    }

    int GetItemQuantity() const
    {
        return m_itemQuantity;
    }

    int GetPrice() const
    {
        return m_price;
    }

    sf::Vector2f GetPos() const
    {
        return m_pos;
    }

    sf::Sprite* GetSprite()
    {
        return m_sprite;
    }

    sf::Sprite* GetBubbleSprite()
    {
        return m_bubbleSprite;
    }

    State GetState()
    {
        return m_state;
    }
    Dialog* GetDialog()
    {
        return m_dialog;
    }

#pragma endregion

#pragma region Setter

    void SetName(std::string _name)
    {
        m_name = _name;
    }

    void SetSentence(std::string _sentence)
    {
        m_sentence = _sentence;
    }

    void SetThanksSentence(std::string _sentence)
    {
        m_thanksSentence = _sentence;
    }

    void SetDenySentence(std::string _sentence)
    {
        m_denySentence = _sentence;
    }

    void SetItem(Item* _item)
    {
        m_item = _item;
    }

    void SetItemQuantity(int _quantity)
    {
        m_itemQuantity = _quantity;
    }

    void SetPrice(int _value)
    {
        m_price = _value;
    }


    void SetPos(sf::Vector2f _pos)
    {
        m_pos = _pos;
    }

    void SetSprite(sf::Sprite* _sprite)
    {
        m_sprite = _sprite;
    }

    void SetBubbleSprite(sf::Sprite* _sprite)
    {
        m_bubbleSprite = _sprite;
    }

    void SetInteractSprite(sf::Sprite* _sprite)
    {
        m_interactSprite = _sprite;
    }

    void SetId(int _id)
    {
        m_id = _id;
    }

    void SetState(State _state)
    {
        m_state = _state;
    }

    void SetDialog(Dialog* _dialog)
    {
        m_dialog = _dialog;
    }


#pragma endregion

    void Update();
    void Display(sf::RenderWindow& _window);
    void ArrivalAnimation(sf::Vector2f);
    void LeavingAnimation(sf::Vector2f);
    virtual void Trade() = 0;
    virtual bool CanTrade() = 0;
    bool CanInteract();

protected:
    int m_id = 0;
    std::string m_name;
    std::string m_sentence;
    std::string m_thanksSentence;
    std::string m_denySentence;
    Item* m_item = nullptr;
    int m_itemQuantity = 0;
    int m_price;
    sf::Sprite* m_sprite = nullptr;
    sf::Sprite* m_bubbleSprite = nullptr;
    sf::Sprite* m_interactSprite = nullptr;
    sf::Vector2f m_pos;
    float m_elapsedTime = 0.f;
    State m_state = WAIT;
    Dialog* m_dialog;
};

#endif // GUEST_H
