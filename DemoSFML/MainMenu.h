#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "GameState.h"


namespace sfe { class Movie; }
class Slider;

class GUIManager;

class MainMenu : public GameState
{
private:
	enum Menus {
		MAIN,
		OPTIONS,
		CREDITS
	};
	GUIManager* m_guiManager = nullptr;
	Slider* m_volumeSlider = nullptr;
	sfe::Movie* m_video = nullptr;
	sfe::Movie* m_creditsVideo = nullptr;
	
	sf::Clock clock;
	Menus m_menuState = MAIN;

public:
	MainMenu();
	void Init() override;
	void Update(float _dt) override;
	void Display(sf::RenderWindow& _window) override;
	void Quit() override;
	void ChangeMenuState(Menus _state);
	void HandleEvents(sf::Event& _ev) override;
};

#endif