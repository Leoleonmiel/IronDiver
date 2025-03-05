#include "Slot.h"
#include <random>
#include <__msvc_chrono.hpp>


void Slot::Flashing(float _dt)
{
	m_elapsedTime += _dt;

	float alphaMax = (m_elapsedTime / 3.0f) * 255.0f;
	alphaMax = std::min(255.0f, alphaMax);

	float alphaMin = 255.0f - ((m_elapsedTime / 3.0f) * 255.0f);
	alphaMin = std::max(0.0f, alphaMin);

	flashing = true;

	if (m_elapsedTime >= 3.0f)
	{
		canChangeState = true;
		m_elapsedTime = 0;
	}
	else
	{
		canChangeState = false;
	}
	sf::Color spriteColor = m_sprite->getColor();

	switch (m_state)
	{
	case VISIBLE:
		spriteColor.a = (sf::Uint8)alphaMin;
		m_sprite->setColor(spriteColor);
		if (canChangeState)
		{
			m_state = HIDDEN;
		}
		break;
	case HIDDEN:
		spriteColor.a = (sf::Uint8)alphaMax;
		m_sprite->setColor(spriteColor);
		if (canChangeState)
		{
			m_state = VISIBLE;
		}
		break;
	default:
		break;
	}
}


void Slot::Update(float _dt)
{
	if (this != nullptr)
	{

		if (!flashing)
		{
			sf::Color spriteColor = m_sprite->getColor();
			spriteColor.a = 255;
			//spriteColor.r = 0; // Red component to 0
			//spriteColor.g = 0; // Green component to 255
			//spriteColor.b = 0; // Blue component to 0
			m_sprite->setColor(spriteColor);
		}
	}
	/*if (m)*/
	//Flashing(_dt);
}

void Slot::Display(sf::RenderWindow& _window)
{
	if (this != nullptr)
	{
		BlitSprite(*m_sprite, sf::Vector2f(1145, 780), 0, _window);

		if (m_item != nullptr)
		{
			if(m_item->_sprite != nullptr)BlitSprite(*m_item->_sprite, sf::Vector2f(1145 + 50, 780 + 50), 0, _window);
			else BlitSprite(*m_item->_slotSprite, sf::Vector2f(1145 + 50, 780 + 50), 0, _window);
		}
	}
}