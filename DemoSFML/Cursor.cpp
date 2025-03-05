#include "Cursor.h"
#include "common.h"
#include "CameraManager.h"

Cursor::Cursor()
{
	m_sprite = new sf::Sprite(LoadSprite("Assets/Sprites/Cursor/Cursor.png", true));
}

Cursor::~Cursor()
{
	delete m_sprite;
	m_sprite = NULL;
}

void Cursor::Display(sf::RenderWindow& _window)
{
	if (m_sprite and m_isVisible)
	{
		_window.setView(_window.getDefaultView());

		m_sprite->setPosition(GameData::GetMousePos());

		_window.draw(*m_sprite);
		_window.setView(*CameraManager::GetInstance()->GetView());

	}
}
