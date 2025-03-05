#ifndef START_STATE_H
#define START_STATE_H

#include "GameState.h"
#include <mutex>
#include <thread>
#include <iostream>
#include "SFML/Graphics.hpp"

namespace std
{
	class thread;
}
class GUIManager;

enum Sequence
{
	FIRST_SEQUENCE,
	START_FIRST_LOGO_SEQUENCE,
	END_FIRST_LOGO_SEQUENCE,
	SECOND_SEQUENCE,
	START_SECOND_LOGO_SEQUENCE,
	END_SECOND_LOGO_SEQUENCE,
	THIRD_SEQUENCE,
};

class StartState : public GameState
{
private:
	//sf::Clock m_clock;
	//sf::Font* m_font = nullptr;
	//sf::Text* m_text = nullptr;
	//GUIManager* m_guiManager = nullptr;

	float m_elapsedTime = 0;
	float m_alpha = 0.f;
	Sequence m_sequence = FIRST_SEQUENCE;
	bool canChangeSequence = false;

	sf::Sprite* m_logoCrea = nullptr;
	sf::Sprite* m_logoStudio = nullptr;

public:
	StartState();
	void Init() override;
	void Update(float _dt) override;
	void Display(sf::RenderWindow& _window) override;
	void HandleEvents(sf::Event& _event) override;
	void Quit() override;

};

#endif