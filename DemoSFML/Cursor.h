#ifndef CURSOR_H
#define CURSOR_H

#include "SFML/Graphics.hpp"

class Cursor
{
public:
	Cursor();
	~Cursor();

	void Display(sf::RenderWindow& _window);

	void SetSprite(sf::Sprite* _sprite) { m_sprite = _sprite; }
	void SetIsVisible(bool _isVisible) { m_isVisible = _isVisible; }
	void SetColor(sf::Color _color) { m_sprite->setColor(_color); }
	void SetScale(float _scale) { m_sprite->setScale(_scale, _scale); }

private:
	sf::Sprite* m_sprite = nullptr;
	bool m_isVisible = false;
};
#endif // !CURSOR_H

