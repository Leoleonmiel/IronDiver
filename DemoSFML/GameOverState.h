#ifndef GAME_OVER_STATE__H
#define GAME_OVER_STATE__H

#include "GameState.h"

namespace sfe { class Movie;}

class GameOverState : public GameState
{
public:
	GameOverState() {}
	void Init() override;
	void Update(float) override;
	void Quit() override;
	void Display(sf::RenderWindow&) override;
	void HandleEvents(sf::Event&) override;

private:
	sf::Sprite* m_background = nullptr;
	float m_timerToQuit = NULL;

	sfe::Movie* m_video = nullptr;
};
#endif
