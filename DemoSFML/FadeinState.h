#ifndef FADE_IN_STATE_H
#define FADE_IN_STATE_H

#include "GameState.h"

class FadeInState : public GameState
{
private:
	float m_fadeTime;
	sf::Color m_fadeColor;

	sf::Clock m_clock;
	sf::RectangleShape m_rectangle;

public:
	FadeInState(float _time = 1.5f, sf::Color _color = sf::Color::Black);
	void Init() override;
	void Update(float _dt) override;
	void Quit() override;
	void Display(sf::RenderWindow& _window) override;
	void HandleEvents(sf::Event& _event) override;
};

#endif