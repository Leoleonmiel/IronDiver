#ifndef INPUT_MANGER_H
#define INPUT_MANGER_H

#include <iostream>
#include "SFML/Graphics.hpp"

#include <unordered_map>

#define NB_JOY_BUTTON 16
		
class InputManager
{
public:
	static InputManager* GetInstance();

	InputManager();
	~InputManager() {}

	void Load();
	void Event(sf::Event& _event);
	void Update(float dt);
	void DisplayInputs();

	bool IsSinglePress(std::string _controlName);
	bool IsLongPress(std::string _controlName);
	bool IsLongRelease(std::string _controlName);

	//Renvoie l'angle de visée du grappin à partir de son centre de pivot
	float GetAimAngle(sf::Vector2f _shooterPos);

	float GetMouseWheelDelta();

	bool m_disableInputs = false;

private:
	static InputManager* Instance;

	//Detect si l'utilisateur à apuyer sur le bouton d'un peripherique et switch sur ce dernier
	void DetectAndSwitchDevice(sf::Event& _event);
	void ReadAndConvertKMInputs();
	void ReadAndAsignActionKMInputs();

	enum Device
	{
		NO_DEVICE = -1,
		KEYBOARD,
		MOUSE_BUTTON,
		MOUSE_WHEEL,
		JOYSTICK,

		NB_DEVICES
	};
	enum Device m_currentDevice = KEYBOARD;

	struct InputAction
	{
		Device device = NO_DEVICE;
		int id = -1;
		bool isSinglePress = false;
		bool releaseSinglePress = false;
		bool isLongPress = false;
		bool currentFrame = false;
		float timeRelease;
	};


	float wheelDelta = 0;

	std::unordered_map<std::string, InputAction> m_inputsConvert;
	std::unordered_map<std::string, InputAction> m_inputsAction;
};

#endif // !INPUT_MANGER_H

//Menu de jeu
//Echap
//
//Fermer les overlays
//Echap
//
//Inventaire
//Tab