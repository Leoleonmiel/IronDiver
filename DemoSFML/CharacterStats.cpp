#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp> 
#include "CharacterStats.h"
#include "GrabHook.h"
#include "InputManager.h"

using json = nlohmann::json;

void CharacterStats::LoadBoostJSON(std::string _pathFile)
{
	std::ifstream file(_pathFile);
	if (!file.is_open())
	{
		std::cerr << "Cannot open JSON File (wrong path ?)." << std::endl;
		return;
	}

	json jsonData;
	try
	{
		file >> jsonData;
	}
	catch (json::parse_error& e)
	{
		std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
		return;
	}

	try
	{
		if (jsonData.find("Upgrade") != jsonData.end())
		{
			auto characterData = jsonData["Upgrade"];

			// Stats load
			if (characterData.find("WalkSpeed") != characterData.end())
			{
				auto statsData = characterData["WalkSpeed"];

				if (statsData.find("P1") != statsData.end())
				{
					if (m_walkBoostLevel == 1)
					{
						m_walkSpeed += statsData["P1"];
						m_maxWalkSpeed += statsData["P11"];
					}
				}
				if (statsData.find("P2") != statsData.end())
				{
					if (m_walkBoostLevel == 2)
					{
						m_walkSpeed += statsData["P2"];
						m_maxWalkSpeed += statsData["P22"];
					}
				}
				if (statsData.find("P3") != statsData.end())
				{
					if (m_walkBoostLevel == 3)
					{
						m_walkSpeed += statsData["P3"];
						m_maxWalkSpeed += statsData["P33"];
					}
				}
			}
			else
			{
				std::cerr << "Error with stats." << std::endl;
				return;
			}	

			if (characterData.find("Damage") != characterData.end())
			{
				auto statsData = characterData["Damage"];

				if (statsData.find("P1") != statsData.end())
				{
					if (m_damageBoostLevel == 1)
					{
						m_damage += statsData["P1"];
					}
				}
				if (statsData.find("P2") != statsData.end())
				{
					if (m_damageBoostLevel == 2)
					{
						m_damage += statsData["P2"];
					}
				}
				if (statsData.find("P3") != statsData.end())
				{
					if (m_damageBoostLevel == 3)
					{
						m_damage += statsData["P3"];
					}
				}
			}
			else
			{
				std::cerr << "Error with stats." << std::endl;
				return;
			}
		}
		else
		{
			std::cerr << "Error with JSON DATA." << std::endl;
			return;
		}
	}
	catch (json::type_error& e)
	{
		std::cerr << "Type error : " << e.what() << std::endl;
		return;
	}
}

void CharacterStats::LoadStatsJSON(std::string _pathFile)
{
	std::ifstream file(_pathFile);
	if (!file.is_open())
	{
		std::cerr << "Cannot open JSON File (wrong path ?)." << std::endl;
		return;
	}

	json jsonData;
	try
	{
		file >> jsonData;
	}
	catch (json::parse_error& e)
	{
		std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
		return;
	}

	try
	{
		if (jsonData.find("Character") != jsonData.end())
		{
			auto characterData = jsonData["Character"];

			// Stats load
			if (characterData.find("Stats") != characterData.end())
			{
				auto statsData = characterData["Stats"];

				if (statsData.find("Health") != statsData.end())
				{
					m_maxHp = statsData["Health"];
				}
				if (statsData.find("Damage") != statsData.end())
				{
					m_damage = statsData["Damage"];
				}
				if (statsData.find("AttackSideSpeed") != statsData.end())
				{
					m_attackSideSpeed = statsData["AttackSideSpeed"];
				}
				if (statsData.find("AttackUpDuration") != statsData.end())
				{
					m_attackUpDuration = statsData["AttackUpDuration"];
				}
				if (statsData.find("AttackDownDuration") != statsData.end())
				{
					m_attackDownDuration = statsData["AttackDownDuration"];
				}
				if (statsData.find("AttackRate") != statsData.end())
				{
					m_attackCooldown = statsData["AttackRate"];
				}
			}
			else
			{
				std::cerr << "Error with stats." << std::endl;
				return;
			}

			// Physics load
			if (characterData.find("Physics") != characterData.end())
			{
				auto statsData = characterData["Physics"];

				if (statsData.find("Density") != statsData.end())
				{
					m_density = statsData["Density"];
				}
				if (statsData.find("Friction") != statsData.end())
				{
					m_friction = statsData["Friction"];
				}

			}
			else
			{
				std::cerr << "Error with Physics." << std::endl;
				return;
			}

			// Movement load
			if (characterData.find("Movement") != characterData.end())
			{
				auto movementData = characterData["Movement"];
				if (movementData.find("WalkSpeed") != movementData.end())
				{
					m_walkSpeed = movementData["WalkSpeed"];
				}
				if (movementData.find("JumpStrenght") != movementData.end())
				{
					m_jumpStrenght = movementData["JumpStrenght"];
				}
				if (movementData.find("JumpMinTime") != movementData.end())
				{
					m_jumpMinTime = movementData["JumpMinTime"];
				}
				if (movementData.find("JumpMaxTime") != movementData.end())
				{
					m_jumpMaxTime = movementData["JumpMaxTime"];
				}
				if (movementData.find("JumpMaxVelocity") != movementData.end())
				{
					m_jumpMaxVelocity = movementData["JumpMaxVelocity"];
				}
				if (movementData.find("MidAirSpeed") != movementData.end())
				{
					m_midAirSpeed = movementData["MidAirSpeed"];
				}
				if (movementData.find("MidAirMaxSpeed") != movementData.end())
				{
					m_midAirMaxSpeed = movementData["MidAirMaxSpeed"];
				}
				if (movementData.find("AttackImpulse") != movementData.end())
				{
					m_attackImpulse = movementData["AttackImpulse"];
				}
				if (movementData.find("MaxWalkSpeed") != movementData.end())
				{
					m_maxWalkSpeed = movementData["MaxWalkSpeed"];
				}

			}
			else
			{
				std::cerr << "Error with Movement." << std::endl;
				return;
			}

			// Hitbox load
			if (characterData.find("Hitbox") != characterData.end())
			{
				auto hitboxData = characterData["Hitbox"];
				if (hitboxData.find("CoreSize") != hitboxData.end())
				{
					auto val = hitboxData["CoreSize"];
					if (val.find("x") != val.end())
					{
						m_coreSize.x = val["x"];
					}
					if (val.find("y") != val.end())
					{
						m_coreSize.y = val["y"];
					}
				}
				if (hitboxData.find("HeadSize") != hitboxData.end())
				{
					auto val = hitboxData["HeadSize"];
					if (val.find("x") != val.end())
					{
						m_headSize.x = val["x"];
					}
					if (val.find("y") != val.end())
					{
						m_headSize.y = val["y"];
					}
				}
				if (hitboxData.find("FeetSize") != hitboxData.end())
				{
					auto val = hitboxData["FeetSize"];
					if (val.find("x") != val.end())
					{
						m_feetSize.x = val["x"];
					}
					if (val.find("y") != val.end())
					{
						m_feetSize.y = val["y"];
					}
				}
				if (hitboxData.find("WallSensorSize") != hitboxData.end())
				{
					auto val = hitboxData["WallSensorSize"];
					if (val.find("x") != val.end())
					{
						m_wallSensorSize.x = val["x"];
					}
					if (val.find("y") != val.end())
					{
						m_wallSensorSize.y = val["y"];
					}
				}
				if (hitboxData.find("AttackSize") != hitboxData.end())
				{
					auto val = hitboxData["AttackSize"];
					if (val.find("x") != val.end())
					{
						m_attackSize.x = val["x"];
					}
					if (val.find("y") != val.end())
					{
						m_attackSize.y = val["y"];
					}
				}
			}
			else
			{
				std::cerr << "Error with Hitbox." << std::endl;
				return;
			}

			// GabHook load
			if (characterData.find("GrabHook") != characterData.end())
			{
				m_grabHookStats = new GrabHookStats();

				auto grabhookData = characterData["GrabHook"];
				if (grabhookData.find("NormalOffset") != grabhookData.end())
				{
					m_grabHookStats->normalOffset = grabhookData["NormalOffset"];
				}
				if (grabhookData.find("RoadEdges") != grabhookData.end())
				{
					m_grabHookStats->roadEdges = grabhookData["RoadEdges"];
				}
				if (grabhookData.find("Cooldown") != grabhookData.end())
				{
					m_grabHookStats->cooldown = grabhookData["Cooldown"];
				}
				if (grabhookData.find("Acceleration") != grabhookData.end())
				{
					m_grabHookStats->acceleration = grabhookData["Acceleration"];
				}
				if (grabhookData.find("RaycastRange") != grabhookData.end())
				{
					m_grabHookStats->raycastRange = grabhookData["RaycastRange"];
				}
				if (grabhookData.find("SpeedMin") != grabhookData.end())
				{
					m_grabHookStats->speedMin = grabhookData["SpeedMin"];
				}
				if (grabhookData.find("SpeedMax") != grabhookData.end())
				{
					m_grabHookStats->speedMax = grabhookData["SpeedMax"];
				}


				if (grabhookData.find("WhiteList") != grabhookData.end())
				{
					if (grabhookData["WhiteList"].is_array())
					{
						for (const auto& item : grabhookData["WhiteList"])
						{
							m_grabHookStats->whiteList.push_back(item.get<std::string>());
						}
					}

				}

				if (grabhookData.find("BlackList") != grabhookData.end())
				{
					if (grabhookData["BlackList"].is_array())
					{
						for (const auto& item : grabhookData["BlackList"])
						{
							m_grabHookStats->blackList.push_back(item.get<std::string>());
						}
					}
				}
			}
			else
			{
				std::cerr << "Error with GabHook." << std::endl;
				return;
			}

			if (characterData.find("FX") != characterData.end())
			{
				auto fxData = characterData["FX"];
				if (fxData.find("ShadowWhiteList") != fxData.end())
				{
					if (fxData["ShadowWhiteList"].is_array())
					{
						for (const auto& item : fxData["ShadowWhiteList"])
						{
							m_shadowWhiteList.push_back(item.get<std::string>());
						}
					}
				}
			}
			else
			{
				std::cerr << "Error with FX." << std::endl;
				return;
			}
		}
		else
		{
			std::cerr << "Error with JSON DATA." << std::endl;
			return;
		}
	}
	catch (json::type_error& e)
	{
		std::cerr << "Type error : " << e.what() << std::endl;
		return;
	}
}

CharacterStats::CharacterStats(std::string _pathStatsFile, std::string _pathBoostFile)
{
	CharacterStats::LoadStatsJSON(_pathStatsFile);
	CharacterStats::LoadBoostJSON(_pathBoostFile);
	m_currentHp = m_maxHp;
}

void CharacterStats::Update()
{
	InputManager& input = *InputManager::GetInstance();

	if (input.IsSinglePress("ReloadPlayerStats"))
	{
		std::cerr << "Character Data Reload" << std::endl;
		LoadStatsJSON("ConfigFiles/Character/CharacterData.json");
		std::cerr << "Character Boost Data Reload" << std::endl;
		LoadBoostJSON("ConfigFiles/Character/CharacterBoostData.json");
	}
}
void CharacterStats::SetWalkBoostLevel(int _level)
{
	m_walkBoostLevel = _level;
}

void CharacterStats::SetDamageBoostLevel(int _level)
{
	m_damageBoostLevel = _level;
}

void CharacterStats::SetCanUseSpecialSpell(bool _value)
{
	m_canUseSpecialSpell = _value;
}

void CharacterStats::SetSpeed(float speed)
{
	m_walkSpeed = speed;
}

void CharacterStats::SetMaxHp(int _maxHp)
{
	m_maxHp = _maxHp;
}

void CharacterStats::SetHp(int _hp)
{
	m_currentHp = _hp;
}

void CharacterStats::AddHealth(int _hp)
{
	m_currentHp += _hp;
	if (m_currentHp > m_maxHp)
	{
		m_currentHp = m_maxHp;
	}
}

void CharacterStats::AddDamage(int _hp)
{
	m_currentHp -= _hp;
	if (m_currentHp < 0)
	{
		m_currentHp = 0;
	}
}


