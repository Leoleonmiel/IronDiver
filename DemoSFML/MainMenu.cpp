#include "MainMenu.h"
#include "LoadingState.h"
#include "MineState.h"
#include "common.h"
#include "TextureManager.h"
#include "GUI.h"
#include "GUIManager.h"
#include <iostream>
#include <fstream>
#include "FadeoutState.h"
#include "InputManager.h"
#include "ItemManager.h"
#include "Cursor.h"
#include "SoundManager.h"
#include "Slider.h"
#include "SaveManager.h"

#include "sfeMovie/Movie.hpp"

MainMenu::MainMenu()
{
	TextureManager::LoadTextures("Assets/Sprites");
	ItemManager::Init("ConfigFiles/Data/ItemData.json");

}

void MainMenu::Init()
{
	SoundManager::GetInstance()->StopAll();
	GUIImage::TextureSearchFunction = &TextureManager::GetTexture;
	GUIText::Font = new sf::Font;
	GUIText::Font->loadFromFile("Assets/Fonts/Orbitron-ExtraBold.ttf");
	m_guiManager = GUIManager::GetInstance();

	std::ifstream dialogsFile("ConfigFiles/UI/MainMenuUI.dat");
	if (dialogsFile.is_open())
	{
		m_guiManager->Load(dialogsFile);
		dialogsFile.close();
	}

	GUIText* tempText = (GUIText*)m_guiManager->GetNodeByTag("PlayText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("PlayButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("CreditsText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("CreditsButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("SettingsText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("SettingsButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("QuitText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("QuitButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("ContinueText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("ContinueButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("fullText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("Fullscre")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("BackText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("BackButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("BackTextCredit");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("BackButtonCredit")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	GameData::Cursor->SetIsVisible(true);

	SoundManager::GetInstance()->Play("MenuTheme");
	m_volumeSlider = new Slider((int)SCREEN_SIZE.x / 2 - 250, (int)SCREEN_SIZE.y / 2 + 100);
	m_volumeSlider->create(0, 100);
	m_volumeSlider->setSliderValue(SoundManager::GetInstance()->GetVolume());

	if (GameData::fullscreen)
	{
		GUIManager::GetInstance()->GetNodeByTag("togglePoint")->SetEnabled(true);
	}
	else
	{
		GUIManager::GetInstance()->GetNodeByTag("togglePoint")->SetEnabled(false);
	}

	m_video = new sfe::Movie();
	m_video->openFromFile("Videos/Video_Menu.mp4");
	m_video->fit(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y);
	m_video->play();

	m_creditsVideo = new sfe::Movie();
	m_creditsVideo->openFromFile("Videos/Video_Credits.mp4");
	m_creditsVideo->fit(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y);
}

void MainMenu::Quit()
{
	GameData::Cursor->SetIsVisible(false);

	m_creditsVideo->stop();
	delete m_creditsVideo;
	m_creditsVideo = nullptr;
	
	m_video->stop();
	delete m_video;
	m_video = nullptr;
}

void MainMenu::ChangeMenuState(Menus _state)
{
	if (_state == m_menuState) return;
	GUI* mainMenu = (GUI*)GUIManager::GetInstance()->GetNodeByTag("MainMenu");
	GUI* optionsMenu = (GUI*)GUIManager::GetInstance()->GetNodeByTag("OptionsMenu");
	GUI* creditsMenu = (GUI*)GUIManager::GetInstance()->GetNodeByTag("CreditsMenu");
	switch (_state)
	{
	case MainMenu::MAIN:
		mainMenu->SetEnabled(true);
		optionsMenu->SetEnabled(false);
		creditsMenu->SetEnabled(false);
		break;
	case MainMenu::OPTIONS:
		mainMenu->SetEnabled(false);
		optionsMenu->SetEnabled(true);
		creditsMenu->SetEnabled(false);
		break;
	case MainMenu::CREDITS:
		mainMenu->SetEnabled(false);
		optionsMenu->SetEnabled(false);
		creditsMenu->SetEnabled(true);
		break;
	default:
		break;
	}
	m_menuState = _state;
}

void MainMenu::Update(float _dt)
{
	auto inputs = InputManager::GetInstance();
	GUIText* tempText = nullptr;
	switch (m_menuState)
	{
	case MainMenu::MAIN:
	{
		GUI* playButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PlayButton");
		GUI* quitButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("QuitButton");
		GUI* optionsButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("SettingsButton");
		GUI* creditsButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("CreditsButton");
		GUI* continueButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("ContinueButton");

		tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("PlayText");
		tempText->Update(_dt, *GameData::Window);
		tempText->SetInMiddle(*playButton->GetSfShape());
		tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("QuitText");
		tempText->Update(_dt, *GameData::Window);
		tempText->SetInMiddle(*quitButton->GetSfShape());
		tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("SettingsText");
		tempText->Update(_dt, *GameData::Window);
		tempText->SetInMiddle(*optionsButton->GetSfShape());
		tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("CreditsText");
		tempText->Update(_dt, *GameData::Window);
		tempText->SetInMiddle(*creditsButton->GetSfShape());
		tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("ContinueText");
		tempText->Update(_dt, *GameData::Window);
		tempText->SetInMiddle(*continueButton->GetSfShape());
		if (inputs->IsSinglePress("Attack"))
		{
			if (playButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), playButton))
			{
				LoadingState::StartLoading<MineState>();
				LoadingState::PushState(new FadeOutState(new LoadingState));
			}
			else if (quitButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), quitButton))
			{
				GameData::Window->close();
			}
			else if (optionsButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), optionsButton))
			{
				ChangeMenuState(MainMenu::OPTIONS);
			}
			else if (creditsButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), creditsButton))
			{
				ChangeMenuState(MainMenu::CREDITS);
				m_video->pause();
				m_creditsVideo->play();
			}
			else if (continueButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), continueButton))
			{
				if (SaveManager::CanLoad()) 
				{
					LoadingState::StartLoading<MineState>();
					LoadingState::PushState(new FadeOutState(new LoadingState));
					SaveManager::loadAfterInit = true;
				}
			}
		}
		break;
	}
	case MainMenu::OPTIONS:
	{
		SoundManager::GetInstance()->SetVolume(m_volumeSlider->getSliderValue());
		GUI* backButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("BackButton");
		GUI* fullscreenToggle = (GUI*)GUIManager::GetInstance()->GetNodeByTag("Fullscre");
		GUI* fullscreenCheckbox = (GUI*)GUIManager::GetInstance()->GetNodeByTag("fullScreenCheckbox");
		tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("BackText");
		tempText->Update(_dt, *GameData::Window);
		tempText->SetInMiddle(*backButton->GetSfShape());
		tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("fullText");
		tempText->Update(_dt, *GameData::Window);
		tempText->SetInMiddle(*fullscreenToggle->GetSfShape());
		m_volumeSlider->Update();
		if (inputs->IsSinglePress("Attack"))
		{
			if (backButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), backButton))
			{
				ChangeMenuState(MainMenu::MAIN);
			}
			if (fullscreenToggle->DetectClick((sf::Vector2i)GameData::GetMousePos(), fullscreenToggle) || fullscreenCheckbox->DetectClick((sf::Vector2i)GameData::GetMousePos(), fullscreenCheckbox))
			{
				GameData::fullscreen = !GameData::fullscreen;
				if (GameData::fullscreen)
				{
					GUIManager::GetInstance()->GetNodeByTag("togglePoint")->SetEnabled(true);
					GameData::Window->create(sf::VideoMode::getDesktopMode(), "IronDiver", sf::Style::Fullscreen);
				}
				else
				{
					GUIManager::GetInstance()->GetNodeByTag("togglePoint")->SetEnabled(false);
					GameData::Window->create(sf::VideoMode::getDesktopMode(), "IronDiver", sf::Style::Default);
				}
				GameData::Window->setFramerateLimit(MAX_FPS);
				GameData::Window->setVerticalSyncEnabled(true);
				GameData::Window->setMouseCursorVisible(false);
			}
		}
		//save settings
		std::ofstream optionFile("ConfigFiles/Options.txt");
		optionFile << "fullscreen " << GameData::fullscreen << std::endl;
		optionFile << "volume " << m_volumeSlider->getSliderValue() << std::endl;
		optionFile.close();
		break;
	}
	case MainMenu::CREDITS:
	{
		GUI* backButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("BackButtonCredit");
		tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("BackTextCredit");
		tempText->Update(_dt, *GameData::Window);
		tempText->SetInMiddle(*backButton->GetSfShape());
		if (inputs->IsSinglePress("Attack"))
		{
			if (backButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), backButton))
			{
				ChangeMenuState(MainMenu::MAIN);
				m_video->play();
				m_creditsVideo->pause();
			}
		}
		break;
	}
	default:
		break;
	}
	SoundManager::GetInstance()->Update();
	//loop video
	if (m_video->getStatus() == sfe::Stopped)
	{
		m_video->play();
	}

	if (m_creditsVideo->getStatus() == sfe::Stopped)
	{
		m_creditsVideo->play();
	}
}

void MainMenu::Display(sf::RenderWindow& _window)
{
	m_video->update();
	m_creditsVideo->update();

	if (m_guiManager != nullptr && m_guiManager->GetCurrentRoot() != nullptr)
	{
		sf::View view = _window.getView();
		_window.setView(_window.getDefaultView());
		if (m_menuState != MainMenu::CREDITS) _window.draw(*m_video);
		else _window.draw(*m_creditsVideo);
		if(m_menuState == MainMenu::OPTIONS) m_volumeSlider->draw(_window);
		m_guiManager->GetCurrentRoot()->Display(_window);
		_window.setView(view);
	}
}

void MainMenu::HandleEvents(sf::Event& _ev)
{
	if (_ev.type == sf::Event::KeyPressed)
	{

	}
}
