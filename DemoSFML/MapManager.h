#ifndef TILED_MANAGER_H
#define TILED_MANAGER_H

#include <map>
#include <vector>
#include "SFML/Graphics.hpp"
#include <utility>

#include "Tileson/tileson.hpp"
#include "Box2D_Shape.h"
#include "Box2D_Collision.h"

class MapManager
{
public:
	static MapManager* GetInstance();
	tson::Map* GetCurrentMapData() { return m_currentMapData; }
	void DisplayMap(sf::RenderWindow& _window, sf::RenderStates _states = sf::RenderStates::Default);
	void DisplayBack(sf::RenderWindow& _window, sf::RenderStates _states = sf::RenderStates::Default);
	void DisplayFront(sf::RenderWindow& _window, sf::RenderStates _states = sf::RenderStates::Default);

	int GetCollidingObjectID(b2::Shape* _collidingShape);

	template<typename T>
	T* GetObjectType(uintptr_t _ptr) { return reinterpret_cast<T*>(_ptr); }

	template<typename T>
	bool isObjectColliding(uintptr_t _objecShapePtr, bool _canBeDeleted)
	{
		T* shape = GetObjectType<T>(_objecShapePtr);
		//shape->GetBody()->GetFixtureList()[0].SetSensor(true);
		if (shape != nullptr)
		{
			int ID = GetCollidingObjectID(shape);
			if (ID != -1)
			{	
				if (_canBeDeleted)
				{
					RemoveObjectByGID(ID);
					return true;
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		return false;
	}

	MapManager() = default;
	~MapManager() = default;

protected:

	std::unordered_map<std::string, std::unordered_map<std::string, sf::FloatRect>> m_zonesMapBounds{};
	std::unordered_map<std::string, std::vector<sf::Vector2f>> m_enemySpawnList{};
	std::unordered_map<std::string, std::vector<sf::Vector2f>> m_teleporterSpawnist{};
	std::map<int, std::string> m_interactibleList{};
	std::map<std::string, tson::Vector2i> m_mapPosOffset{};
	sf::Vector2f m_playerSpawn{};
	sf::Font m_font{};
	std::vector<sf::Text> m_textList{};
	std::string m_currentMapName{};
	std::string m_previousMapName{};

	void LoadMap(std::string _mapName);
	void AddMap(std::unique_ptr<tson::Map> _mapData, std::string _filename);
	void ResetMapManager();
	void DeleteB2Hitbox();
	void UpdateMap(float _dt);



	tson::Map* m_currentMapData{};
	std::unordered_map<std::string, std::unique_ptr<tson::Map>> m_mapHashList{};

	bool m_isMapChanged{ false };
	float m_backgroundZoomFactor{ 1.f };
	std::map<int, b2::Shape*> m_b2ShapeHashList{};
private:
	struct TileSpriteSettings
	{
		const sf::Texture& texture;
		const sf::IntRect& textureRegion;
		sf::Vector2f pos;
		bool hortFlip;
		bool vertFlip;
		bool diagonalFlip;
		int tileID;
		std::string texName;
	};

	static MapManager* Instance;

	void LoadEventsInformation(std::string _layerName);
	void LoadZonesInformation(std::string _layerName);


	void InitLayers();

	void CreateMapTiles(tson::Layer* _childLayer);
	void CreateB2HitboxOfLayer(tson::Layer* _childLayer);
	void CreateObjectOfLayer(tson::Layer* _childLayer);
	void CreateObjectOfLayer(tson::Layer* _childLayer, std::string _parentLayer);
	void CreateTextOfLayer(std::string _layerName);
	void CreateTileSprite(TileSpriteSettings& params);

	//void CreateGroupeOfLayer(std::string _layerName);

	sf::Vector2f GetTileOffset(int _tileId, tson::Vector2i _objSize, tson::Tileset* _tileset);

	void DisplayLayer(sf::RenderWindow& _window, std::string _fileName, std::string _layerName ,sf::RenderStates _states = sf::RenderStates::Default);
	void DisplayLayer(sf::RenderWindow& _window, tson::Layer* _childLayer, std::string _parentLayerName, sf::RenderStates _states = sf::RenderStates::Default);
	void DisplayLayer(sf::RenderWindow& _window, tson::Layer* _childLayer, sf::RenderStates _states = sf::RenderStates::Default);

	bool IsInsideView(const sf::Vector2f& _pos, tson::Vector2i  _objSize);

	void RemoveObjectByGID(int _gid);



	//Tile related
	std::map<uint32_t, tson::Animation*> m_animationUpdateQueue{};
	std::map<std::string, std::unordered_map<int, sf::Sprite>> m_spriteAtlasList{};

	//Map related
	std::vector<b2::Shape*> m_b2ShapeList{};

};

#endif // !TILED_MANAGER_H



