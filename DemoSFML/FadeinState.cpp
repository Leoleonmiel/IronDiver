#include "FadeinState.h"
#include "common.h"

FadeInState::FadeInState(float _time, sf::Color _color)
{
	m_fadeTime = _time;
	m_fadeColor = _color;
}

void FadeInState::Init()
{
	m_clock.restart();

	m_rectangle.setSize(SCREEN_SIZE);
	m_rectangle.setPosition({ 0,0 });
	m_rectangle.setOrigin({ 0,0 });
	m_rectangle.setFillColor(sf::Color(m_fadeColor.r, m_fadeColor.g, m_fadeColor.b, 255));

}

void FadeInState::Update(float _dt)
{
	if (m_clock.getElapsedTime().asSeconds() < m_fadeTime)
	{
		float fade = m_clock.getElapsedTime().asSeconds() / (m_fadeTime);
		if (fade > 1.f)
		{
			fade = 1.f;
		}
		else if (fade < 0.f)
		{
			fade = 0.f;
		}
		fade = 1 - fade;
		m_rectangle.setFillColor(sf::Color(m_fadeColor.r, m_fadeColor.g, m_fadeColor.b, (int)(255 * fade)));
	}
	else
	{
		PopState();
		return;
	}
}

void FadeInState::Quit()
{
}

void FadeInState::Display(sf::RenderWindow& _window)
{
	const sf::View view = GameData::Window->getView();
	GameData::Window->setView(GameData::Window->getDefaultView());
	_window.draw(m_rectangle);
	GameData::Window->setView(view);
}

void FadeInState::HandleEvents(sf::Event& _event)
{
}
