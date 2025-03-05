#include "InputManager.h"
#include "common.h"
#include <fstream>

#define RELEASE_TIME 0.3f

using namespace sf;

InputManager* InputManager::Instance = nullptr;

InputManager* InputManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new InputManager();
	}
	return Instance;
}

InputManager::InputManager()
{
	Load();
}

void InputManager::Load()
{
	m_inputsConvert.clear();
	m_inputsAction.clear();

	ReadAndConvertKMInputs();
	ReadAndAsignActionKMInputs();
}

void InputManager::Event(sf::Event& _event)
{
	//GameData::gameData.window.setKeyRepeatEnabled(false);

	DetectAndSwitchDevice(_event);
	wheelDelta = 0;

	for (auto& input : m_inputsAction)
	{
		InputAction& action = input.second;

		switch (action.device)
		{
		case KEYBOARD:
			if (_event.type == sf::Event::KeyPressed and !m_disableInputs)
			{
				if (_event.key.code == sf::Keyboard::Key(action.id))
				{
					action.isSinglePress = !action.isLongPress;
					action.isLongPress = true;
				}
			}
			if (_event.type == sf::Event::KeyReleased)
			{
				if (_event.key.code == sf::Keyboard::Key(action.id))
				{
					//action.isSinglePress = false;
					action.isLongPress = false;
				}
			}
			break;
		case MOUSE_BUTTON:
			if (_event.type == sf::Event::MouseButtonPressed and !m_disableInputs)
			{
				if (_event.key.code == sf::Keyboard::Key(action.id))
				{
					action.isSinglePress = !action.isLongPress;
					action.isLongPress = true;
				}
			}
			if (_event.type == sf::Event::MouseButtonReleased)
			{
				if (_event.key.code == sf::Keyboard::Key(action.id))
				{
					action.isLongPress = false;
				}
			}
			break;
		case MOUSE_WHEEL:
			if (_event.type == sf::Event::MouseWheelScrolled and !m_disableInputs)
			{
				wheelDelta = _event.mouseWheelScroll.delta;
				action.isSinglePress = !action.isLongPress;
				action.isLongPress = true;
			}
			else 
			{
				action.isLongPress = false;
			}
			break;
		case JOYSTICK:
			break;

		default:
			break;
		}
	}
}

void InputManager::Update(float dt)
{
	for (auto& input : m_inputsAction)
	{
		InputAction& action = input.second;

		if (action.isSinglePress)
		{
			action.timeRelease += GameData::DeltaTime;
			if (action.timeRelease > RELEASE_TIME)
			{
				action.isSinglePress = false;
				action.timeRelease = 0;
			}
			else
			{
				action.releaseSinglePress = true;
			}
		}
		if (!action.isSinglePress && action.currentFrame) action.currentFrame = false;
	}
}

void InputManager::DisplayInputs()
{

}

bool InputManager::IsSinglePress(std::string _controlName)
{
	if (!m_inputsAction[_controlName].isSinglePress && m_inputsAction[_controlName].currentFrame) return true;
	if (m_inputsAction[_controlName].isSinglePress)
	{
		m_inputsAction[_controlName].isSinglePress = false;
		m_inputsAction[_controlName].currentFrame = true;
		m_inputsAction[_controlName].timeRelease = 0;
		return true;
	}
	return false;
}

bool InputManager::IsLongPress(std::string _controlName)
{
	return m_inputsAction[_controlName].isLongPress;
}

bool InputManager::IsLongRelease(std::string _controlName)
{
	return !m_inputsAction[_controlName].isLongPress;
}

float InputManager::GetAimAngle(Vector2f _shooterPos)
{
	Vector2f targetPos = GameData::GetMousePos();
	return GetAngle(_shooterPos, targetPos);
}

float InputManager::GetMouseWheelDelta()
{
	return wheelDelta;
}

void InputManager::DetectAndSwitchDevice(sf::Event& _event)
{
	if (_event.type == sf::Event::MouseButtonPressed or
		_event.type == sf::Event::KeyPressed)
	{
		m_currentDevice = KEYBOARD;
	}

	if (_event.type == sf::Event::JoystickButtonPressed)
	{
		m_currentDevice = JOYSTICK;
	}
}

void InputManager::ReadAndConvertKMInputs()
{
	std::ifstream fileInputsList("ConfigFiles/Inputs/InputsList.txt");

	if (fileInputsList.is_open())
	{
		std::string line;
		std::string input;
		int countLine = 0;
		int SFML_ID = 0;
		Device deviceReaded = KEYBOARD;

		// Parcourir les lignes du fichier
		do
		{
			fileInputsList >> input;

			if (countLine < Keyboard::KeyCount)
			{
				SFML_ID = countLine;
				deviceReaded = KEYBOARD;
			}
			else if (countLine < Keyboard::KeyCount + Mouse::ButtonCount)
			{
				SFML_ID = countLine - Keyboard::KeyCount;
				deviceReaded = MOUSE_BUTTON;
			}
			else if (countLine < Keyboard::KeyCount + Mouse::ButtonCount + 2)
			{
				SFML_ID = countLine - Keyboard::KeyCount + Mouse::ButtonCount;
				deviceReaded = MOUSE_WHEEL;
			}

			m_inputsConvert[input].id = SFML_ID;
			m_inputsConvert[input].device = deviceReaded;

			countLine++;

		} while (std::getline(fileInputsList, line));

		fileInputsList.close();
	}
	else
	{
		std::cerr << "Erreur lors de l'ouverture du fichier : InputsList.txt" << std::endl;
	}
}

void InputManager::ReadAndAsignActionKMInputs()
{
	std::ifstream fileUserInputsKM("ConfigFiles/Inputs/UserInputsKM.txt");

	if (fileUserInputsKM.is_open())
	{
		std::string line;
		std::string actionName;
		std::string inputName;
		size_t pos = 0;

		// Parcourir les lignes du fichier
		while (std::getline(fileUserInputsKM, line))
		{
			// Trouver la position du ":"
			pos = line.find(":");
			if (pos != std::string::npos)
			{
				// Extraire le mot avant ":"
				actionName = line.substr(0, pos);
				// Extraire le mot après ":"
				inputName = line.substr(pos + 2); // +2 pour ignorer l'espace après ":"

				if (m_inputsConvert.find(inputName) != m_inputsConvert.end())
				{
					m_inputsAction[actionName] = m_inputsConvert[inputName];
				}
			}
		}
		fileUserInputsKM.close();
	}
	else
	{
		std::cerr << "Erreur lors de l'ouverture du fichier : UserInputsKM.txt" << std::endl;
	}
}
