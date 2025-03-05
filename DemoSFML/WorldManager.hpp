#ifndef WORLDMANAGER_HPP
#define WORLDMANAGER_HPP

#include <map>
#include "SFML/Graphics.hpp"

#include "Tileson/external/nlohmann.hpp"
#include "Tileson/extras/pocketlzma.hpp"
#include "Tileson/tileson.hpp"
#include <Tileson/json/NlohmannJson.hpp>

#include "MapManager.h"

class MapManager;

class WorldManager : public MapManager
{
public:
	static WorldManager* GetInstance();
	WorldManager() = default;
	~WorldManager() = default;

	void Init(const fs::path& _projectPath = fs::path("Assets/Tiled/content/test-maps"));
	void Update(float _dt);
	void Display(sf::RenderWindow& _window);


	sf::Vector2f GetPlayerWorldPos() const { return m_playerSpawn; }
	std::unordered_map<std::string, std::unordered_map<std::string, sf::FloatRect>> GetZonesWorldBounds() const { return m_zonesMapBounds; }
	std::unordered_map<std::string, std::vector<sf::Vector2f>> GetEnemySpawnList() const { return m_enemySpawnList; }
	std::unordered_map<std::string, std::vector<sf::Vector2f>> GetTeleporterSpawnList() const { return m_teleporterSpawnist; }
	std::map<int, std::string> GetInteractibleList() const { return m_interactibleList; }
	std::map<int, b2::Shape*> GetB2ShapeHashList() const { return m_b2ShapeHashList; }
	std::map<std::string, tson::WorldMapData> GetWorldDataMap() const { return m_worldDataMap; }
	tson::WorldMapData GetWorldData(const std::string& _mapName) const { return m_worldDataMap.at(_mapName); }

	std::string GetCurrentMapName() const { return m_currentMapName; }
	std::string GetPreviousMapName() const { return m_previousMapName; }
	void SetCurrentMapName(const std::string& _mapName) { m_currentMapName = _mapName; }
	void SetPreviousMapName(const std::string& _mapName) { m_previousMapName = _mapName; }

	void SetCurrentMapData(const std::string& _mapName) { m_currentMapData = m_mapHashList[_mapName].get(); }
	void SetZoomFactor(float _factor) { m_backgroundZoomFactor = _factor; }

	void ResetWorld();
	void ParseProject(const std::string& _filename = "ProjectT.tiled-project");
protected:
	//Project Related
	tson::Project m_project{ std::make_unique<tson::NlohmannJson>() };
	fs::path m_basePath{};

private:
	static WorldManager* m_instance;


	//Project Related
	fs::path m_projectPath{};

	//World Related
	std::map<std::string, bool> m_worldVisibilityMap{}; //World-related data in project
	std::map<std::string, tson::WorldMapData> m_worldDataMap{};
	std::vector<std::string> m_worldMapInfo{};

	sf::Vector2f m_playerSpawnWorld{};

};

#endif // !WORLDMANAGER_HPP
