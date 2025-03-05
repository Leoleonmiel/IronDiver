#include "Guest.h"
#include "common.h"
#include <iostream>

Guest::~Guest()
{
    delete m_sprite;
}

void Guest::Display(sf::RenderWindow& _window)
{
    m_sprite->setScale(0.40f, 0.40f);

    // Vérifier si l'opacité du sprite n'est pas déjà égale à 255

    //else
    //{
    //    canChangeSequence = false;
    //}

    // Afficher le sprite avec la nouvelle opacité
    //BlitSprite(*m_sprite, m_pos, 0, _window);

    if (m_state == READY_TO_INTERACT)
    {
        /* BlitSprite(*m_bubbleSprite, sf::Vector2f(m_pos.x - 150, m_pos.y - 200), 0, _window);
        BlitSprite(*m_interactSprite, sf::Vector2f(m_pos.x - 150, m_pos.y - 300), 0, _window);
        BlitSprite(*m_itemToBuy->_sprite, sf::Vector2f(m_pos.x - 150, m_pos.y - 210), 0, _window);*/
    }
    else if (m_state == BUSY)
    {
        // BlitSprite(*m_itemToBuy->_sprite, sf::Vector2f(960, 360), 0, _window);
    }
}


void Guest::Update(/*sf::Vector2f _endPos*//*, float _xOffset, float _yOffset*/)
{
    m_elapsedTime += GameData::DeltaTime;

    // Calculer l'opacité en fonction du temps écoulé
    float alphaMax = (m_elapsedTime / 2.0f) * 255.0f;
    alphaMax = std::min(255.0f, alphaMax);

    // Limiter l'opacité à 255 une fois qu'elle atteint ce niveau
    alphaMax = std::min(alphaMax, 255.0f);

    // Mettre à jour la couleur du sprite avec l'opacité calculée
    sf::Color color = m_sprite->getColor();
    color.a = static_cast<sf::Uint8>(alphaMax);
    m_sprite->setColor(color);

   /* bool firstInLine = _xOffset == 0;

    _endPos.x += _xOffset;
    _endPos.y += _yOffset;*/

    //switch (m_state)
    //{
    //case Guest::WAIT:
    //    if (m_pos != _endPos)
    //    {
    //        m_state = WALK_TO_SHOP;
    //    }
    //    break;

    //case Guest::WALK_TO_SHOP:
    //  /*  if (m_pos != _endPos)
    //    {
    //        ArrivalAnimation(_endPos);
    //    }
    //    else
    //    {*/
    //        ArrivalAnimation(_endPos);
    //        if (firstInLine)
    //        {
    //            m_state = READY_TO_INTERACT;
    //            //  m_state = WALK_TO_EXIT;
    //        }
    //        else
    //        {
    //            m_state = WAIT;
    //        }

    //        
    //    
    //    break;
    //case Guest::BUSY:
    //    break;
    //case Guest::READY_TO_INTERACT:
    //    break;
    //case Guest::WALK_TO_EXIT:
    //    LeavingAnimation(sf::Vector2f(2100, 600));
    //    break;
    //case Guest::READY_TO_LEAVE:
    //    break;
    //default:
    //    break;
    //}


}

void Guest::ArrivalAnimation(sf::Vector2f _endPos)
{
    /*if (m_elapsedTime >= 5.0f)
    {
        return;
    }

    m_elapsedTime += GameData::DeltaTime;

    float t = m_elapsedTime / 10.0f;

    m_pos = m_pos * (1 - t) + _endPos * t;*/
}


void Guest::LeavingAnimation(sf::Vector2f _endPos)
{
   /* if (m_elapsedTime >= 10)
    {
        m_state = READY_TO_LEAVE;
        return;
    }

    m_elapsedTime += GameData::DeltaTime;

    float t = m_elapsedTime / 700.f;
    m_pos = m_pos * (1 - t) + _endPos * t;*/
}

bool Guest::CanInteract()
{
    return m_state == State::READY_TO_INTERACT;
}

bool Guest::CanTrade()
{
    return true;
}

void Guest::Trade()
{
    
}











