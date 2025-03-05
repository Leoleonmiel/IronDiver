#include "common.h"
#include "InputManager.h"
#include "Box2D_World.h"
#include "Cursor.h"
#include "MainMenu.h"
#include "MineState.h"
#include "Inventory.h"
#include <fstream>
#include "StartState.h"
#include "LoadingState.h"
#include "ShopState.h"
#include "SoundManager.h"
#include "CharacterStats.h"
//#include "FxManager.h"

void LoadOptions()
{
	std::ifstream file("ConfigFiles/Options.txt");
	if (!file.is_open())
	{
		//Create the txt
		std::ofstream optionFile("ConfigFiles/Options.txt");
		optionFile << "fullscreen 0" << std::endl;
		optionFile << "volume 100" << std::endl;
		optionFile.close();
	}
	else
	{
		std::string line;
		while (std::getline(file, line))
		{
			if (line.find("fullscreen") != std::string::npos)
			{
				std::string value = line.substr(line.find(" ") + 1);
				GameData::fullscreen = std::stoi(value);
			}
			if (line.find("volume") != std::string::npos)
			{
				std::string value = line.substr(line.find(" ") + 1);
				SoundManager::GetInstance()->SetVolume(std::stoi(value));
			}
		}
	}
}

int main(void)
{
	srand(unsigned(time(nullptr)));
#pragma warning(disable: 6031)
	rand(); rand(); rand();
#pragma warning(default: 6031)

	LoadOptions();

	if (GameData::fullscreen)
	{
		GameData::Window = new sf::RenderWindow(sf::VideoMode((unsigned int)SCREEN_SIZE.x, (unsigned int)SCREEN_SIZE.y), "Iron Diver", sf::Style::Fullscreen);
	}
	else
	{
		GameData::Window = new sf::RenderWindow(sf::VideoMode((unsigned int)SCREEN_SIZE.x, (unsigned int)SCREEN_SIZE.y), "Iron Diver", sf::Style::Default);
	}

	InputManager* input = InputManager::GetInstance();

	sf::RenderWindow& window = *GameData::Window;
	window.setFramerateLimit(MAX_FPS);
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(false);

#ifdef _DEBUG
	window.setMouseCursorVisible(true);
#endif

	GameData::GameSpeed = 1.0f;
	GameData::Cursor = new Cursor;
	GameData::CharacterStats = new CharacterStats("ConfigFiles/Character/CharacterData.json", "ConfigFiles/Character/CharacterBoostData.json");

	LoadingState::StartLoading<StartState>();
	//LoadingState::StartLoading<ShopState>();
	//LoadingState::StartLoading<MineState>();
	GameState::ChangeState(new LoadingState());

	Inventory::shopInventory = new ShopInventory();
	SoundManager::GetInstance()->Init("Assets/Sounds/SoundsData.txt");

	//input->Load();

	while (window.isOpen())
	{
		GameData::ClockTime = GameData::Clock.restart();
		GameData::DeltaTime = GameData::ClockTime.asSeconds();
		GameData::GameTime += GameData::DeltaTime;
	
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			input->Event(event);
			GameState::ActiveState->HandleEvents(event);
		}

		input->Update(GameData::DeltaTime);
		//FxManager::GetInstance()->Update(GameData::DeltaTime);
		GameState::ActiveState->Update(GameData::DeltaTime);

		GameState::DisplayStates(window);
		
	}
	return EXIT_SUCCESS;
}