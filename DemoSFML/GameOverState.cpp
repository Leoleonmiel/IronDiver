#include "GameOverState.h"
#include "TextureManager.h"
#include "ShopState.h"
#include "FadeoutState.h"
#include "LoadingState.h"
#include "sfeMovie/Movie.hpp"


#define GAMEOVER_TIMER 3.0f

void GameOverState::Init()
{
	m_background = TextureManager::CreateSprite("Lose.png");

	m_video = new sfe::Movie();
	m_video->openFromFile("Videos/Video_GameOver.mp4");
	m_video->fit(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y);
	m_video->play();

}

void GameOverState::Quit()
{
	m_video->stop();
	delete m_video;
	m_video = nullptr;
}

void GameOverState::Update(float _dt)
{
	m_video->update();
	m_timerToQuit += _dt;
	if (m_timerToQuit >= (m_video->getDuration().asSeconds() - 2.f))
	{

		LoadingState::StartLoading<ShopState>();
		PushState(new FadeOutState(new LoadingState()));
	}
}

void GameOverState::Display(sf::RenderWindow& _window)
{
	const sf::View view = _window.getView();
	_window.setView(_window.getDefaultView());
	_window.draw(*m_video);
	//_window.draw(*m_background);
	_window.setView(view);
}

void GameOverState::HandleEvents(sf::Event& _event)
{
	if (_event.type == sf::Event::KeyPressed)
	{
	}
}

