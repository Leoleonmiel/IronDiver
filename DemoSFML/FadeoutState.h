#ifndef FADE_OUT_STATE_H
#define FADE_OUT_STATE_H

#include "GameState.h"

class FadeOutState : public GameState
{
private:
	GameState* m_nextState;
	float m_fadeTime;
	sf::Color m_fadeColor;

	sf::Clock m_clock;
	sf::RectangleShape m_rectangle;
	bool m_blocking = true;
public:
	FadeOutState(GameState* _nextState, bool _blocking = true, float _time = 1.5f, sf::Color _color = sf::Color::Black);
	void Init() override;
	void Update(float _dt) override;
	void Quit() override;
	void Display(sf::RenderWindow& _window) override;
	void HandleEvents(sf::Event& _event) override;
};

#endif