#include "FadeoutState.h"
#include "common.h"



FadeOutState::FadeOutState(GameState* _nextState, bool _blocking, float _time, sf::Color _color)
{
	m_nextState = _nextState;
	m_fadeTime = _time;
	m_fadeColor = _color;
	m_blocking = _blocking;
	m_rectangle.setFillColor(sf::Color(m_fadeColor.r, m_fadeColor.g, m_fadeColor.b, 0));
}

void FadeOutState::Init()
{
	m_clock.restart();
	
	m_rectangle.setSize(SCREEN_SIZE);
	m_rectangle.setPosition({ 0,0 });
	m_rectangle.setOrigin({ 0,0 });


}

void FadeOutState::Update(float _dt)
{
	if (m_clock.getElapsedTime().asSeconds() < m_fadeTime)
	{
		float fade = m_clock.getElapsedTime().asSeconds() / (m_fadeTime * 0.9f);
		if (fade > 1.f)
		{
			fade = 1.f;
		}
		m_rectangle.setFillColor(sf::Color(m_fadeColor.r, m_fadeColor.g, m_fadeColor.b, (int)(255 * fade)));
		if (!m_blocking)
		{
			StateStack.at(StateStack.size() - 2)->Update(_dt);
		}
	}
	else
	{
		ChangeState(m_nextState);
		return;
	}
}

void FadeOutState::Quit()
{
}

void FadeOutState::Display(sf::RenderWindow& _window)
{
	const sf::View view = GameData::Window->getView();
	GameData::Window->setView(GameData::Window->getDefaultView());
	_window.draw(m_rectangle);
	GameData::Window->setView(view);
}

void FadeOutState::HandleEvents(sf::Event& _event)
{
}


