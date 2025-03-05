#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>

enum ButtonMenu
{
	MAIN,
	CREDITS
};

/// <summary>
/// Abstract class representing the current state of the game (main menu, game etc...)
/// <para> Gives access to Init, Update, Quit and Display methods.</para>
/// </summary>
class GameState
{
protected:
	static std::vector<GameState*> StateStack;
	bool m_paused = false;

public:
	/// <summary>
	/// Pointer to the state currently active
	/// </summary>
	static GameState* ActiveState;

	static void InitStack();

	static void PushState(GameState* state);
	static GameState* PopState();
	static void ChangeState(GameState* state);
	static void SetStateStack(std::vector<GameState*> _stateStack);

	static void DisplayStates(sf::RenderWindow& window);

	/// <summary>
	/// First method called when changing states, it serves to update and load any necessary part for the state
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// Method called every frame, usually serving to advance the game
	/// </summary>
	virtual void Update(float _dt) = 0;

	/// <summary>
	/// Method called when popping the state outside of the stack.
	/// <para> Serve to handle any loose ends that might cause bugs </para>
	/// </summary>
	virtual void Quit() = 0;

	/// <summary>
	/// Method called to handle display
	/// </summary>
	/// <param name="window"> Reference to the game's main render window. Can't draw on nothing, now, can we? </param>
	virtual void Display(sf::RenderWindow& window) = 0;

	virtual void HandleEvents(sf::Event& event) = 0;

	void SetPause(bool pause)
	{
		m_paused = pause;
	}

	bool Paused() const
	{
		return m_paused;
	}
};


#endif

