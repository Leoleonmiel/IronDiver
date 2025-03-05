#ifndef LOADING_STATE_H
#define LOADING_STATE_H

#include "GameState.h"
#include "StartState.h"
#include <mutex>
#include <thread>
#include <iostream>

#include "Animator.h"
#include "Animation.h"

namespace std
{
	class thread;
}

class LoadingState : public GameState
{
private:
	sf::Clock m_clock;
	sf::Font* m_font = nullptr;
	sf::Text* m_text = nullptr;

	Animator* m_animator = nullptr;
	Animation* m_anim = nullptr;

	sf::Sprite m_sprite;
	sf::Texture* m_texture = nullptr;

	static std::thread* loadingThread;
	static GameState* LoadedState;
	static bool Loading;
	static std::mutex StateMutex;

	template <class T>
	static void ThreadLoad()
	{
		T* localState = new T();

		StateMutex.lock();
		LoadedState = localState;
		Loading = false;
		StateMutex.unlock();
	}
public:
	sf::Vector2f GetAnimationPos();

	template <class T>
	static void StartLoading()
	{
		static_assert(std::is_base_of<GameState, T>::value, "T must be derived from GameState");
		StateMutex.lock();
		Loading = true;
		StateMutex.unlock();
		
		loadingThread = new std::thread(ThreadLoad<T>);
	}
	LoadingState();
	void Init() override;
	void Update(float _dt) override;
	void Display(sf::RenderWindow& _window) override;
	void HandleEvents(sf::Event& _event) override;
	void Quit() override;
};

#endif