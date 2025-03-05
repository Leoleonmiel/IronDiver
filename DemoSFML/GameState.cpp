#include "GameState.h"
#include <iostream>
#include "common.h"
#include "LoadingState.h"
#include "Cursor.h"

//GameState-----------------------GameState--------------------GameState------------------
std::vector<GameState*> GameState::StateStack;

GameState* GameState::ActiveState;

void GameState::InitStack()
{
	StateStack = *new std::vector<GameState*>();
	ActiveState = (GameState*)nullptr;
}

void GameState::PushState(GameState* state)
{
	StateStack.push_back(state);
	ActiveState = StateStack[StateStack.size() - 1];
	ActiveState->Init();
}

/// <summary>
/// Remove the top-state from the stack
/// </summary>
/// <returns> Returns a pointer to the state that got popped</returns>
GameState* GameState::PopState()
{
	StateStack[StateStack.size() - 1]->Quit();
	GameState* state = StateStack[StateStack.size() - 1];
	StateStack.pop_back(); 
	ActiveState = StateStack[StateStack.size() - 1];
	return state;
}

void GameState::ChangeState(GameState* _state)
{
	GameState* state;

	while (StateStack.size() != 0)
	{
		state = StateStack[StateStack.size() - 1];
		state->Quit();
		StateStack.pop_back();
		delete(state);
	}

	StateStack.push_back(_state);

	ActiveState = StateStack[StateStack.size() - 1];
	ActiveState->Init();
}

void GameState::SetStateStack(std::vector<GameState*> _stateStack)
{
	for (auto it = StateStack.rbegin(); it != StateStack.rend(); it++)
	{
		(*it)->Quit();
		delete (*it);
	}

	StateStack.clear();

	for (auto it = _stateStack.begin(); it != _stateStack.end(); it++)
	{
		StateStack.push_back(*it);
		(*it)->Init();
	}

	ActiveState = StateStack[StateStack.size() - 1];
}

void GameState::DisplayStates(sf::RenderWindow& window)
{
	window.clear(sf::Color::Black);

	for (int i = 0; i < StateStack.size(); i++)
	{
		StateStack[i]->Display(window);
	}

	for (auto& dp : DebugPoint::ListDebugPoint)
	{
		window.draw(*dp);
	}

	GameData::Cursor->Display(window);
	window.display();
}