#include "SaveManager.h"
#include <ctime>
#include <filesystem>
#include <iostream>
#include "Inventory.h"
#include "ItemManager.h"
#include "CharacterController.h"
#include "CharacterStats.h"
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;

std::string SaveManager::LastSavePath = "";
bool SaveManager::loadAfterInit = false;
#pragma warning(disable : 4996)
void SaveManager::Save()
{
	std::time_t t = std::time(nullptr);
	std::tm* now = std::localtime(&t);
	char dateStr[80];
	std::strftime(dateStr, 80, "%d_%m_%y_%H-%M-%S", now);

	std::string folderPath = "ConfigFiles/Save/" + std::string(dateStr);
	if (!fs::create_directory(folderPath))
	{
		std::cerr << "Erreur lors de la création du dossier." << std::endl;
	}
	Inventory::playerInventory->AddItem(ItemManager::GetItem("Rock"), 1);
	Inventory::shopInventory->AddItem(ItemManager::GetItem("Rock"), 1);
	Inventory::playerInventory->Save(folderPath + "/PlayerInventory.json");
	Inventory::shopInventory->Save(folderPath + "/ShopInventory.json");
	SaveCharacter(folderPath + "/Player.json");
	std::cout << "saved" << std::endl;

	LastSavePath = folderPath;

	std::ofstream fichier("ConfigFiles/Save/folder.txt");

	if (!fichier.is_open())
	{
		std::cerr << "Erreur : Impossible de créer le fichier." << std::endl;
		return;
	}

	fichier << folderPath;

	fichier.close();

}



void SaveManager::SaveCharacter(std::string _path)
{
	nlohmann::json playerData;

	playerData["Health"] = GameData::CharacterStats->GetHp();
	playerData["DamageBoost"] = GameData::CharacterStats->GetDamageBoostLevel();
	playerData["SpeedBoost"] = GameData::CharacterStats->GetWalkBoostLevel();
	playerData["CanUseSpecialAttack"] = GameData::CharacterStats->CanUseSpecialSpell();
	playerData["PositionX"] = GameData::CharaCtrl->GetPos().x;
	playerData["PositionY"] = GameData::CharaCtrl->GetPos().y;

	std::fstream file(_path, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		file << playerData;
		file.close();
	}
}

bool SaveManager::CanLoad()
{
	std::string path = LastSavePath;

	if (path == "")
	{
		std::ifstream fichier("ConfigFiles/Save/folder.txt");

		if (!fichier.is_open()) {
			std::cerr << "Erreur : Impossible d'ouvrir le fichier. (SaveManager at l82)" << std::endl;
			return false;
		}

		std::string firstLine;
		std::getline(fichier, firstLine);

		path = firstLine;

		// Ferme le fichier
		fichier.close();
	}

	std::fstream file(path + "/Player.json", std::ios::in);

	if (!file.is_open()) 
	{
		return false;
	}
	file.close();
	return true;
}

void SaveManager::Load()
{
	nlohmann::json shopInvData;

	std::string path = LastSavePath;

	if (path == "")
	{
		std::ifstream fichier("ConfigFiles/Save/folder.txt");

		if (!fichier.is_open()) {
			return;
		}

		std::string firstLine;
		std::getline(fichier, firstLine);

		path = firstLine;

		fichier.close();
	}

	std::fstream file(path + "/Player.json", std::ios::in);

	sf::Vector2f savedPos;

	if (file.is_open())
	{
		file >> shopInvData;
		file.close();
		for (auto i = shopInvData.begin(); i != shopInvData.end(); i++)
		{
			if (i.key() == "CanUseSpecialAttack")
			{
				GameData::CharacterStats->SetCanUseSpecialSpell(i.value());
				continue;
			}
			if (i.key() == "DamageBoost")
			{
				GameData::CharacterStats->SetDamageBoostLevel(i.value());
				continue;
			}
			if (i.key() == "Health")
			{
				GameData::CharacterStats->SetHp(i.value());
				continue;
			}

			if (i.key() == "SpeedBoost")
			{
				GameData::CharacterStats->SetWalkBoostLevel(i.value());
				continue;
			}

			if (i.key() == "PositionX")
			{
				savedPos.x = i.value();
				continue;
			}
			if (i.key() == "PositionY")
			{
				savedPos.y = i.value();
				continue;
			}
		}

	}

	GameData::CharaCtrl->SetPos(savedPos);

	Inventory::playerInventory->Load(LastSavePath + "/PlayerInventory.json");
	Inventory::shopInventory->Load(LastSavePath + "/ShopInventory.json");

	loadAfterInit = false;
}