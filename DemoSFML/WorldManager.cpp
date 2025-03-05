#include "WorldManager.hpp"
#include "MapManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
//#define MapManager MapManager::GetInstance();
#include "Tileson/vera_font.h"


WorldManager* WorldManager::m_instance = nullptr;

WorldManager* WorldManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new WorldManager();
	}
	return m_instance;
}

void WorldManager::Init(const fs::path& _projectPath)
{
	m_basePath = _projectPath;
	m_projectPath = fs::path(m_basePath / "project");

	ParseProject();
	sf::Font tempFont;
	tempFont.loadFromMemory(vera_font::_VERA_TTF, vera_font::_VERA_TTF_SIZE);
	m_font = tempFont;

	CameraManager* m_camera = CameraManager::GetInstance();

	m_currentMapName = "Map_tutoriel.json";
	m_currentMapData = m_mapHashList[m_currentMapName].get();
}

void WorldManager::ParseProject(const std::string& _filename)
{
	using namespace std::string_literals; //Used for s-suffix
	tson::Tileson tilesonHelper;

	if (m_project.parse(fs::path(m_projectPath / _filename)))
	{
		for (const auto& folder : m_project.getFolders())
		{
			if (folder.hasWorldFile())
			{
				const auto& world = folder.getWorld();
				for (const auto& data : world.getMapData())
				{
					std::unique_ptr<tson::Map> map = tilesonHelper.parse(fs::path(world.getFolder() / data.fileName));
					if (map->getStatus() == tson::ParseStatus::OK)
					{
						AddMap(std::move(map), data.fileName);
						m_worldDataMap.insert({ data.fileName, data });
						m_worldVisibilityMap.insert({ data.fileName, false });
						std::string info = "Part of .world file (inside a project) '"s + world.getPath().filename().string() + "': " + data.fileName;
						m_worldMapInfo.emplace_back(info);

						//map position
						m_mapPosOffset.insert({ data.fileName, {data.position.x, data.position.y} });
						LoadMap(data.fileName);
					}
					else
					{
						std::cerr << "Parse World error: " << map->getStatusMessage() << std::endl;
					}
				}
			}
		}
	}
	else
	{
		std::cerr << "Project parse error! " << std::endl;
	}
}

void WorldManager::Update(float _dt)
{
	m_worldVisibilityMap[m_currentMapName] = true;
	m_worldVisibilityMap[m_previousMapName] = false;


	//Update(_dt);
	UpdateMap(_dt);
}

void WorldManager::Display(sf::RenderWindow& _window)
{
	//DisplayMap(_window);
}


void WorldManager::ResetWorld()
{
	ResetMapManager();
	//for (auto& [name, data] : m_worldDataMap)
	//{
	//	DeleteCurrentMap(name);
	//}
	m_worldDataMap.clear();
	m_worldVisibilityMap.clear();
	m_worldMapInfo.clear();
	m_worldMapInfo.shrink_to_fit();

	m_currentMapName.clear();
	m_projectPath.clear();
	m_basePath.clear();
	m_project = tson::Project(std::make_unique<tson::NlohmannJson>());
}

