
#include <mutex>
#include <thread>
#include <chrono>
#include <queue>
#include "LoadingState.h"
#include "MineState.h"
#include "ShopState.h"
#include "FadeinState.h"

#include "common.h"
#include "TextureManager.h"

std::thread* LoadingState::loadingThread;
GameState* LoadingState::LoadedState;
bool  LoadingState::Loading;
std::mutex  LoadingState::StateMutex;

sf::Vector2f LoadingState::GetAnimationPos()
{
	return { SCREEN_SIZE.x * 0.7f, SCREEN_SIZE.y * 0.9f };
}

LoadingState::LoadingState()
{
	GetPositionFunc* func = new GetPositionFunc();
	*func = std::bind(&LoadingState::GetAnimationPos, this);
	m_animator = new Animator(func);
}

void LoadingState::Init()
{
	TextureManager::LoadTextures("Assets/Sprites/Player");

	std::queue<Animation*> animations;

	m_font = new sf::Font();
	m_font->loadFromFile("Assets/Fonts/Orbitron-ExtraBold.ttf");
	m_text = new sf::Text();
	m_text->setFillColor(sf::Color::White);
	m_text->setPosition({ SCREEN_SIZE.x * 0.75f, SCREEN_SIZE.y * 0.9f });
	m_text->setCharacterSize(60);
	m_text->setString("Now Loading");
	m_text->setFont(*m_font);

	m_texture = TextureManager::GetTexture("Player_Spritesheet.png");
	if (m_texture == nullptr)
	{
		return;
	}
	m_sprite.setTextureRect({ 0,0,200,200 });
	m_sprite.setOrigin({ 100,100 });
	m_sprite.setScale({ 1.f,1.f });
	m_sprite.setTexture(*m_texture);

	m_anim = new Animation(CreateAnimation(m_sprite, *m_texture, 2, 12.f, { 0,200,200,200 }));

	animations.push(m_anim);

	m_animator->SetAnimationQueue(animations);

}

void LoadingState::Update(float _dt)
{
	if (StateMutex.try_lock())
	{
		if (!Loading)
		{
			std::vector<GameState*> states;
			
			StateMutex.unlock();
			loadingThread->join();

			states.push_back(LoadedState);
			states.push_back(new FadeInState());
			SetStateStack(states);

			return;
		}

		StateMutex.unlock();
	}
}

void LoadingState::Display(sf::RenderWindow& _window)
{
	int dotCount;
	std::string string = "Now Loading";

	dotCount = m_clock.getElapsedTime().asSeconds() * 1.1f;
	dotCount = dotCount % 4;

	for (int i = 0; i < dotCount; i++)
	{
		string += ".";
	}

	_window.clear(sf::Color::Black);
	m_text->setString(string);
	m_animator->Display(_window);
	_window.draw(*m_text);
}


void LoadingState::HandleEvents(sf::Event& _event)
{
	
}

void LoadingState::Quit()
{
	delete m_animator;
	delete m_anim;
	delete m_font;
	delete m_text;
	delete loadingThread;
}
