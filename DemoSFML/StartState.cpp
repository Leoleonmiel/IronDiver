
#include "common.h"
#include "FadeinState.h"
#include "FadeoutState.h"
#include "InputManager.h"
#include "LoadingState.h"
#include "MainMenu.h"
#include "StartState.h"
#include "TextureManager.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

StartState::StartState()
{
	TextureManager::LoadTextures("Assets/Sprites");

	m_logoCrea = TextureManager::CreateSprite("logo-Creajeux.png", true);
	m_logoCrea->setPosition(SCREEN_SIZE / 2.f);
	m_logoCrea->setScale(0.5f, 0.5f);

	m_logoStudio = TextureManager::CreateSprite("logo.png", true);
	m_logoStudio->setPosition(SCREEN_SIZE / 2.f);
	m_logoStudio->setScale(1.5f, 1.5f);
}

void StartState::Init()
{
}

void StartState::Update(float _dt)
{
	InputManager& input = *InputManager::GetInstance();

	if (input.IsSinglePress("Interact"))
	{
		LoadingState::StartLoading<MainMenu>();
		PushState(new FadeOutState(new LoadingState()));
	}


	m_elapsedTime += _dt;

	float alphaMax = (m_elapsedTime / 2.0f) * 255.0f;
	alphaMax = std::min(255.0f, alphaMax);

	float alphaMin = 255.0f - ((m_elapsedTime / 2.0f) * 255.0f);
	alphaMin = std::max(0.0f, alphaMin);

	if (m_elapsedTime >= 2.0f)
	{
		canChangeSequence = true;
		m_elapsedTime = 0;
	}
	else
	{
		canChangeSequence = false;
	}

	switch (m_sequence)
	{
	case FIRST_SEQUENCE:
		if (canChangeSequence)
		{
			m_sequence = START_FIRST_LOGO_SEQUENCE;
		}
		break;
	case START_FIRST_LOGO_SEQUENCE:
		m_alpha = alphaMax;
		if (canChangeSequence)
		{
			m_sequence = END_FIRST_LOGO_SEQUENCE;
		}
		break;
	case END_FIRST_LOGO_SEQUENCE:
		m_alpha = alphaMin;
		if (canChangeSequence)
		{
			m_sequence = SECOND_SEQUENCE;
		}
		break;
	case SECOND_SEQUENCE:
		if (canChangeSequence)
		{
			m_sequence = START_SECOND_LOGO_SEQUENCE;
		}
		break;
	case START_SECOND_LOGO_SEQUENCE:
		m_alpha = alphaMax;
		if (canChangeSequence)
		{
			m_sequence = END_SECOND_LOGO_SEQUENCE;
		}
		break;
	case END_SECOND_LOGO_SEQUENCE:
		m_alpha = alphaMin;
		if (canChangeSequence)
		{
			m_sequence = THIRD_SEQUENCE;
		}
		break;
	case THIRD_SEQUENCE:
		if (canChangeSequence)
		{
			LoadingState::StartLoading<MainMenu>();
			PushState(new FadeOutState(new LoadingState()));

			//std::vector<GameState*> states;
			//states.push_back(new MainMenu());
			//states.push_back(new FadeInState());
			//SetStateStack(states);
		}
		break;
	default:
		break;
	}
}

void StartState::Display(sf::RenderWindow& _window)
{
	if (m_sequence == START_FIRST_LOGO_SEQUENCE or m_sequence == END_FIRST_LOGO_SEQUENCE)
	{
		m_logoCrea->setColor(sf::Color(255, 255, 255, (sf::Uint8)m_alpha));
		_window.draw(*m_logoCrea);
	}
	else if (m_sequence == START_SECOND_LOGO_SEQUENCE or m_sequence == END_SECOND_LOGO_SEQUENCE)
	{
		m_logoStudio->setColor(sf::Color(255, 255, 255, (sf::Uint8)m_alpha));
		_window.draw(*m_logoStudio);
	}
}


void StartState::HandleEvents(sf::Event& _event)
{

}

void StartState::Quit()
{
	delete m_logoCrea;
	m_logoCrea = nullptr;
	delete m_logoStudio;
}
