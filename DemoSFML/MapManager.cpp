#include "MapManager.h"
#include "Box2D_Shape.h"
#include "common.h"
#include "TextureManager.h"
#include <filesystem>
#include "CameraManager.h"
#include "Box2D_Collision.h"
#include "BossChase.h"
#include "Boss.h"

MapManager* MapManager::Instance = nullptr;

MapManager* MapManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new MapManager();
	}
	return Instance;
}
void MapManager::AddMap(std::unique_ptr<tson::Map> _mapData, std::string _filename)
{
	m_mapHashList.emplace(_filename, std::move(_mapData));
}
void MapManager::LoadMap(std::string _mapName)
{
	m_currentMapName = _mapName;
	m_currentMapData = m_mapHashList[_mapName].get();

	InitLayers();
}

void MapManager::InitLayers()
{
	std::vector<tson::Layer> layers = m_currentMapData->getLayers();

	for (auto& parentlayer : layers)
	{

		if (parentlayer.getType() == tson::LayerType::TileLayer)
		{
			CreateMapTiles(&parentlayer);
		}
		else if (parentlayer.getType() == tson::LayerType::ObjectGroup)
		{
			CreateObjectOfLayer(&parentlayer);
		}
		else if (parentlayer.getType() == tson::LayerType::Group)
		{
			for (auto& childLayer : parentlayer.getLayers())
			{
				if (childLayer.getType() == tson::LayerType::TileLayer)
				{
					CreateMapTiles(&childLayer);
				}
				else if (childLayer.getType() == tson::LayerType::ObjectGroup)
				{
					if (parentlayer.getName() == "Interactibles")
					{
						for (auto& childLayer : parentlayer.getLayers())
						{
							CreateB2HitboxOfLayer(&childLayer);
						}
					}
					CreateObjectOfLayer(&childLayer, parentlayer.getName());
				}
				else if (childLayer.getType() == tson::LayerType::Group)
				{
					for (auto& grandChildLayer : childLayer.getLayers())
					{
						if (grandChildLayer.getType() == tson::LayerType::TileLayer)
						{
							CreateMapTiles(&grandChildLayer);
						}
						else if (grandChildLayer.getType() == tson::LayerType::ObjectGroup)
						{
							CreateObjectOfLayer(&grandChildLayer);
						}

						if (childLayer.getName() == "Colliders")
						{
							for (auto& grandChildLayer : childLayer.getLayers())
							{
								CreateB2HitboxOfLayer(&grandChildLayer);
							}
						}
					}
				}
			}
			LoadEventsInformation(parentlayer.getName());
			LoadZonesInformation(parentlayer.getName());
		}
	}
}

void MapManager::UpdateMap(float _dt)
{

	//for (auto& [id, animation] : m_animationUpdateQueue)
	//{
	//	animation->update(_dt);
	//}

	//tson::Layer* layer = m_mapHashList[m_currentMapName]->getLayer("Collectables");

	//if (layer != nullptr)
	//{
	//	for (auto& obj : layer->getObjects())
	//	{
	//		//levitate object of name Energy
	//		if (obj.getName() == "Energy")
	//		{
	//			tson::Tileset* tileset = layer->getMap()->getTilesetByGid(obj.getGid());
	//			uint32_t id = obj.getGid() - tileset->getFirstgid() + 1;
	//			tson::Tile* tile = tileset->getTile(id);
	//			const std::string& name = tileset->getName();
	//			sf::Sprite& sprite = m_spriteAtlasList[name][id];

	//			sf::Vector2f origin = sprite.getOrigin();
	//			sf::Vector2f position = { (float)(obj.getPosition().x + origin.x), (float)(obj.getPosition().y - origin.y) };

	//			position.y += 1000.f * sin(-10.f * _dt);
	//			//sprite.setPosition(position);

	//			std::cout << "pos : " << position.x << " " << position.y << std::endl;
	//		}
	//	}
	//}



}

void MapManager::RemoveObjectByGID(int _gid)
{
	tson::Layer* parentLayer = m_mapHashList[m_currentMapName]->getLayer("Interactibles");

	for (auto& childLayer : parentLayer->getLayers())
	{
		for (auto& obj : childLayer.getObjects())
		{
			if (obj.getId() == _gid)
			{
				auto* ptr = m_b2ShapeHashList.find(obj.getId())->second;
				delete ptr;
				m_b2ShapeHashList.erase(obj.getId());
				childLayer.removeObjectByID(obj.getId());
				break;
			}
		}
	}
}

int MapManager::GetCollidingObjectID(b2::Shape* collidingShape)
{
	for (auto& [id, shape] : m_b2ShapeHashList)
	{
		if (shape == collidingShape)
		{
			return id; // Return the ID of the colliding shape
		}
	}

	return -1; // Return -1 if no match is found
}

void MapManager::ResetMapManager()
{
	m_currentMapName.clear();

	DeleteB2Hitbox();
	for (auto& mapList : m_mapHashList)
	{
		mapList.second.reset();
	}
	m_mapHashList.clear();

	for (auto& spriteList : m_spriteAtlasList)
	{
		spriteList.second.clear();
	}
	m_spriteAtlasList.clear();

	m_textList.clear();
	m_textList.shrink_to_fit();

	for (auto& zoneBounds : m_zonesMapBounds)
	{
		zoneBounds.second.clear();
	}
	m_zonesMapBounds.clear();

	for (auto& enemySpawn : m_enemySpawnList)
	{
		enemySpawn.second.clear();
	}
	m_enemySpawnList.clear();

	m_mapPosOffset.clear();
	m_animationUpdateQueue.clear();
}

#pragma region Display
void MapManager::DisplayMap(sf::RenderWindow& _window, sf::RenderStates _states)
{
	std::vector<tson::Layer> layer = m_currentMapData->getLayers();

	for (auto& parentLayer : layer)
	{
		if (parentLayer.isVisible())
		{
			if (parentLayer.getType() == tson::LayerType::TileLayer)
			{
				DisplayLayer(_window, &parentLayer, _states);
			}
			else if (parentLayer.getType() == tson::LayerType::ObjectGroup)
			{
				DisplayLayer(_window, &parentLayer, _states);
			}
			else if (parentLayer.getType() == tson::LayerType::Group)
			{
				if (parentLayer.getName() == "Interactibles")
				{
					for (auto& childLayer : parentLayer.getLayers())
					{
						if (childLayer.getType() == tson::LayerType::TileLayer)
						{
							DisplayLayer(_window, &childLayer, _states);
						}
						else if (childLayer.getType() == tson::LayerType::ObjectGroup)
						{
							DisplayLayer(_window, &childLayer, parentLayer.getName(), _states);
						}
					}
				}
			}
		}
	}
}

void MapManager::DisplayBack(sf::RenderWindow& _window, sf::RenderStates _states)
{
	std::vector<tson::Layer> layer = m_currentMapData->getLayers();

	for (auto& subLayer : layer)
	{
		if (subLayer.isVisible())
		{
			if (subLayer.getType() == tson::LayerType::Group)
			{
				if (subLayer.getName() == "BackLayers")
				{
					for (auto& subGroupLayer : subLayer.getLayers())
					{
						if (subGroupLayer.isVisible())
						{
							std::cout << "Name : " << subGroupLayer.getName() << std::endl;
							if (subGroupLayer.getType() == tson::LayerType::TileLayer)
							{
								//std::cout << "Adress : " << &subGroupLayer << " " << subGroupLayer.getName() << std::endl;
								DisplayLayer(_window, &subGroupLayer, _states);
							}
							else if (subGroupLayer.getType() == tson::LayerType::ObjectGroup)
							{
								DisplayLayer(_window, &subGroupLayer, _states);
							}
						}
					}
				}
			}
		}
	}
}

void MapManager::DisplayFront(sf::RenderWindow& _window, sf::RenderStates _states)
{
	std::vector<tson::Layer> layer = m_currentMapData->getLayers();

	for (auto& parentLayer : layer)
	{
		if (parentLayer.isVisible())
		{
			if (parentLayer.getType() == tson::LayerType::Group)
			{
				if (parentLayer.getName() == "FrontLayers")
				{
					for (auto& childLayer : parentLayer.getLayers())
					{
						if (childLayer.getType() == tson::LayerType::TileLayer)
						{
							DisplayLayer(_window, &childLayer, _states);
						}
						else if (childLayer.getType() == tson::LayerType::ObjectGroup)
						{
							DisplayLayer(_window, &childLayer, _states);
						}
					}
				}
			}
		}
	}
}
void MapManager::DisplayLayer(sf::RenderWindow& _window, std::string _fileName, std::string _layerName, sf::RenderStates _states)
{
	tson::Layer* layer = m_mapHashList[_fileName]->getLayer(_layerName);

	if (layer != nullptr)
	{
		sf::Vector2f mapPos = { (float)m_mapPosOffset[m_currentMapName].x, (float)m_mapPosOffset[m_currentMapName].y };

		if (layer->getType() == tson::LayerType::TileLayer)
		{
			for (auto& [pos, tileObject] : layer->getTileObjects())
			{
				int tileUniqueIDTileset = tileObject.getTile()->getId();
				const std::string& tilesetName = tileObject.getTile()->getTileset()->getName();
				sf::Sprite& sprite = m_spriteAtlasList[tilesetName][tileUniqueIDTileset];
				sf::Vector2f position = { (float)(tileObject.getPosition().x), (float)(tileObject.getPosition().y) };

				//float objWidth = (float)tileObject.getTile()->getTileSize().x;
				//float objHeight = (float)tileObject.getTile()->getTileSize().y;

				sf::Vector2f origin = sprite.getOrigin();
				position += origin;
				sprite.setPosition(position + mapPos);
				if (IsInsideView(position + mapPos, tileObject.getTile()->getTileSize()))
				{
					_window.draw(sprite);
				}
			}
		}
		else if (layer->getType() == tson::LayerType::ObjectGroup)
		{
			//sf::Vector2f offsetPoint = { (float)layer->getOffset().x, (float)layer->getOffset().y };
			//Timotee: set offsetPoint to mapPos
			sf::Vector2f cameraPos = CameraManager::GetInstance()->GetView()->getCenter();
			//sf::Vector2f offsetPoint = mapPos;
			sf::Vector2f offsetPoint = { (float)layer->getMap()->getParallaxOrigin().x,(float)layer->getMap()->getParallaxOrigin().y };
			sf::Vector2f parallaxFactor = { (float)layer->getParallax().x, (float)layer->getParallax().y };
			sf::Vector2f parallaxOffset = cameraPos - offsetPoint;

			parallaxOffset.x *= (1 - parallaxFactor.x);
			parallaxOffset.y *= (1 - parallaxFactor.y);

			bool hasParallax = layer->getProperties().hasProperty("Parallax");
			if (layer->getName() != "Text")
			{
				for (auto& obj : layer->getObjects())
				{
					tson::Tileset* tileset = layer->getMap()->getTilesetByGid(obj.getGid());
					uint32_t id = obj.getGid() - tileset->getFirstgid() + 1;
					tson::Tile* tile = tileset->getTile(id);
					const std::string& name = tileset->getName();
					sf::Sprite& sprite = m_spriteAtlasList[name][id];

					sf::Vector2f origin = sprite.getOrigin();
					sf::Vector2f position;

					//sf::Vector2f position = { (float)(obj.getPosition().x), (float)(obj.getPosition().y) };
					position = { (float)(obj.getPosition().x + origin.x), (float)(obj.getPosition().y - origin.y) };
					sprite.setRotation(obj.getRotation());
					//{
					//	position = { (float)(obj.getPosition().x + (obj.getSize().x)), (float)(obj.getPosition().y) };
					//}


					if (hasParallax)
					{
						sprite.setPosition((position + mapPos) + parallaxOffset);
					}
					else
					{

						if (obj.getName() == "Background")
						{
							sprite.setPosition(cameraPos);
						}
						else
						{
							sprite.setPosition(position + mapPos);
							sf::Vector2f objScale = { 1.f, 1.f };
							sf::IntRect textureRegion;
							sf::Vector2f offset = GetTileOffset(obj.getGid(), obj.getSize(), tileset);
							textureRegion = { static_cast<int>(offset.x),
																static_cast<int>(offset.y),
																obj.getSize().x,
																obj.getSize().y };

							objScale.x = (float)obj.getSize().x / (float)textureRegion.width;
							objScale.y = (float)obj.getSize().y / (float)textureRegion.height;
							sf::Vector2f scale = { objScale.x, objScale.y };
							if (obj.hasFlipFlags(tson::TileFlipFlags::Horizontally))
							{
								scale.x = -scale.x;
							}
							if (obj.hasFlipFlags(tson::TileFlipFlags::Vertically))
							{
								scale.y = -scale.y;
							}
							sprite.setScale(scale);
						}
					}

					if (IsInsideView(position + mapPos, obj.getSize()) and obj.getName() != "Background")
					{
						_window.draw(sprite);
					}
					else
					{

						_window.draw(sprite);

					}
				}
			}
			else
			{
				for (auto& m_text : m_textList)
				{
					_window.draw(m_text);
				}
			}
		}
	}
	else
	{
		//std::cerr << "Layer " << _layerName << " not found in map " << _fileName << std::endl;
		return;
	}
}


void MapManager::DisplayLayer(sf::RenderWindow& _window, tson::Layer* _childLayer, std::string _parentLayerName, sf::RenderStates _states)
{
	//tson::Layer* layer = m_mapHashList[m_currentMapName]->getLayer(_layerName);

	//std::cout << "Adress DisplayLayer : " << _layer << " " << _layerName << std::endl;
	if (_childLayer != nullptr)
	{
		sf::Vector2f mapPos = { (float)m_mapPosOffset[m_currentMapName].x, (float)m_mapPosOffset[m_currentMapName].y };

		if (_childLayer->getType() == tson::LayerType::TileLayer)
		{
			for (auto& [id, tileObject] : _childLayer->getTileObjects())
			{
				int tileUniqueIDTileset = tileObject.getTile()->getId();
				const std::string& tilesetName = tileObject.getTile()->getTileset()->getName();
				sf::Sprite& sprite = m_spriteAtlasList[tilesetName][tileUniqueIDTileset];
				sf::Vector2f position = { (float)(tileObject.getPosition().x), (float)(tileObject.getPosition().y) };

				//float objWidth = (float)tileObject.getTile()->getTileSize().x;
				//float objHeight = (float)tileObject.getTile()->getTileSize().y;

				if (IsInsideView(position + mapPos, tileObject.getTile()->getTileSize()))
				{
					sf::Vector2f origin = sprite.getOrigin();
					position += origin;
					sprite.setPosition(position + mapPos);
					_window.draw(sprite, _states);
				}
			}
		}
		else if (_childLayer->getType() == tson::LayerType::ObjectGroup)
		{
			//sf::Vector2f offsetPoint = { (float)layer->getOffset().x, (float)layer->getOffset().y };
			//Timotee: set offsetPoint to mapPos
			sf::Vector2f cameraPos = CameraManager::GetInstance()->GetView()->getCenter();
			m_backgroundZoomFactor = CameraManager::GetZoom();
			//sf::Vector2f offsetPoint = mapPos;
			sf::Vector2f offsetPoint = { (float)_childLayer->getMap()->getParallaxOrigin().x,(float)_childLayer->getMap()->getParallaxOrigin().y };
			sf::Vector2f parallaxFactor = { (float)_childLayer->getParallax().x, (float)_childLayer->getParallax().y };
			sf::Vector2f parallaxOffset = cameraPos - offsetPoint;

			parallaxOffset.x *= (1 - parallaxFactor.x);
			//parallaxOffset.y *= (1 - parallaxFactor.y);
			parallaxOffset.y = 0.f;

			bool hasParallax = _childLayer->getProperties().hasProperty("Parallax");
			if (_childLayer->getName() != "Text")
			{
				for (auto& obj : _childLayer->getObjects())
				{
					tson::Tileset* tileset = _childLayer->getMap()->getTilesetByGid(obj.getGid());
					uint32_t id = obj.getGid() - tileset->getFirstgid() + 1;
					tson::Tile* tile = tileset->getTile(id);
					const std::string& name = tileset->getName();
					sf::Sprite* sprite = &m_spriteAtlasList[name][id];

					sf::Vector2f origin = sprite->getOrigin();
					sf::Vector2f position;

					position = { (float)(obj.getPosition().x + origin.x), (float)(obj.getPosition().y - origin.y) };

					//sf::Vector2f position = { (float)(obj.getPosition().x), (float)(obj.getPosition().y) };
					//position = { (float)(obj.getPosition().x + origin.x), (float)(obj.getPosition().y - origin.y) };
					sprite->setRotation(obj.getRotation());

					if (obj.getRotation() == 90)
					{
						position.y += 128.f;
						position.x = position.x - (origin.x - obj.getRotation()) - 30.f;
					}
					else if (obj.getRotation() == 180 or obj.getRotation() == -180)
					{
						position.x -= 128.f;
						position.y += 128.f;
					}
					else if (obj.getRotation() == -90)
					{
						position.x -= 128.f;

					}
					//{
					//	position = { (float)(obj.getPosition().x + (obj.getSize().x)), (float)(obj.getPosition().y) };
					//}


					if (hasParallax)
					{
						position = (position + mapPos) + parallaxOffset;
					}
					else
					{
						if (obj.getName() == "Background")
						{
							position = cameraPos;
							sprite->setScale(m_backgroundZoomFactor, m_backgroundZoomFactor);
						}
						else
						{
							position = position + mapPos;
							sf::Vector2f objScale = { 1.f, 1.f };
							sf::IntRect textureRegion;
							sf::Vector2f offset = GetTileOffset(obj.getGid(), obj.getSize(), tileset);
							textureRegion = { static_cast<int>(offset.x),
																static_cast<int>(offset.y),
																obj.getSize().x,
																obj.getSize().y };

							objScale.x = (float)obj.getSize().x / (float)textureRegion.width;
							objScale.y = (float)obj.getSize().y / (float)textureRegion.height;
							sf::Vector2f scale = { objScale.x, objScale.y };
							if (obj.hasFlipFlags(tson::TileFlipFlags::Horizontally))
							{
								scale.x = -scale.x;
							}
							if (obj.hasFlipFlags(tson::TileFlipFlags::Vertically))
							{
								scale.y = -scale.y;
							}
							sprite->setScale(scale);
						}
					}
					sprite->setPosition(position);

					if (IsInsideView(sprite->getPosition(), obj.getSize()))
					{
						_window.draw(*sprite, _states);
					}
					else if (obj.getName() == "Background")
					{
						_window.draw(*sprite, _states);
					}
				}
			}
			else
			{
				for (auto& m_text : m_textList)
				{
					_window.draw(m_text);
				}
			}
		}
	}
	else
	{
		//std::cerr << "Layer " << _layerName << " not found in map " << _fileName << std::endl;
		return;
	}
}
void MapManager::DisplayLayer(sf::RenderWindow& _window, tson::Layer* _childLayer, sf::RenderStates _states)
{
	if (_childLayer != nullptr)
	{
		sf::Vector2f mapPos = { (float)m_mapPosOffset[m_currentMapName].x, (float)m_mapPosOffset[m_currentMapName].y };

		if (_childLayer->getType() == tson::LayerType::TileLayer)
		{
			for (auto& [id, tileObject] : _childLayer->getTileObjects())
			{
				int tileUniqueIDTileset = tileObject.getTile()->getId();
				const std::string& tilesetName = tileObject.getTile()->getTileset()->getName();
				sf::Sprite& sprite = m_spriteAtlasList[tilesetName][tileUniqueIDTileset];
				sf::Vector2f position = { (float)(tileObject.getPosition().x), (float)(tileObject.getPosition().y) };
				tson::Tile* tile = tileObject.getTile();
				sf::Vector2f objScale = { 1.f, 1.f };
				sf::Vector2f scale = { objScale.x, objScale.y };
				if (tile->hasFlipFlags(tson::TileFlipFlags::Horizontally))
				{
					scale.x = -scale.x;
				}
				if (tile->hasFlipFlags(tson::TileFlipFlags::Vertically))
				{
					scale.y = -scale.y;
				}
				sprite.setScale(scale);
				if (IsInsideView(position + mapPos, tileObject.getTile()->getTileSize()))
				{
					sf::Vector2f origin = sprite.getOrigin();
					position += origin;
					sprite.setPosition(position + mapPos);
					_window.draw(sprite, _states);
				}
			}
		}
		else if (_childLayer->getType() == tson::LayerType::ObjectGroup)
		{
			//sf::Vector2f offsetPoint = { (float)layer->getOffset().x, (float)layer->getOffset().y };
			//Timotee: set offsetPoint to mapPos
			sf::Vector2f cameraPos = CameraManager::GetInstance()->GetView()->getCenter();
			m_backgroundZoomFactor = CameraManager::GetZoom();
			//std::cout << "Zoom : " << m_backgroundZoomFactor << std::endl;
			//sf::Vector2f offsetPoint = mapPos;
			sf::Vector2f offsetPoint = { (float)_childLayer->getMap()->getParallaxOrigin().x,(float)_childLayer->getMap()->getParallaxOrigin().y };
			sf::Vector2f parallaxFactor = { (float)_childLayer->getParallax().x, (float)_childLayer->getParallax().y };
			sf::Vector2f parallaxOffset = cameraPos - offsetPoint;

			parallaxOffset.x *= (1 - parallaxFactor.x);
			//parallaxOffset.y *= (1 - parallaxFactor.y);
			parallaxOffset.y = 0.f;

			bool hasParallax = _childLayer->getProperties().hasProperty("Parallax");
			if (_childLayer->getName() != "Text")
			{
				for (auto& obj : _childLayer->getObjects())
				{
					tson::Tileset* tileset = _childLayer->getMap()->getTilesetByGid(obj.getGid());
					uint32_t id = obj.getGid() - tileset->getFirstgid() + 1;
					tson::Tile* tile = tileset->getTile(id);
					const std::string& name = tileset->getName();
					sf::Sprite& sprite = m_spriteAtlasList[name][id];

					sf::Vector2f origin = sprite.getOrigin();
					sf::Vector2f position;

					position = { (float)(obj.getPosition().x + origin.x), (float)(obj.getPosition().y - origin.y) };

					//sf::Vector2f position = { (float)(obj.getPosition().x), (float)(obj.getPosition().y) };
					//position = { (float)(obj.getPosition().x + origin.x), (float)(obj.getPosition().y - origin.y) };
					sprite.setRotation(obj.getRotation());

					if (obj.getRotation() == 90)
					{
						position.y += 128.f;
						position.x = position.x - (origin.x - obj.getRotation()) - 30.f;
					}
					else if (obj.getRotation() == 180 or obj.getRotation() == -180)
					{
						position.x -= 128.f;
						position.y += 128.f;
					}
					else if (obj.getRotation() == -90)
					{
						position.x -= 128.f;

					}
					//{
					//	position = { (float)(obj.getPosition().x + (obj.getSize().x)), (float)(obj.getPosition().y) };
					//}


					if (hasParallax)
					{
						position = (position + mapPos) + parallaxOffset;
					}
					else
					{
						if (obj.getName() == "Background")
						{
							position = cameraPos;
							sprite.setScale(m_backgroundZoomFactor, m_backgroundZoomFactor);
						}
						else
						{
							position = position + mapPos;
							sf::Vector2f objScale = { 1.f, 1.f };
							sf::IntRect textureRegion;
							sf::Vector2f offset = GetTileOffset(obj.getGid(), obj.getSize(), tileset);
							textureRegion = { static_cast<int>(offset.x),
																static_cast<int>(offset.y),
																obj.getSize().x,
																obj.getSize().y };

							objScale.x = (float)obj.getSize().x / (float)textureRegion.width;
							objScale.y = (float)obj.getSize().y / (float)textureRegion.height;
							sf::Vector2f scale = { objScale.x, objScale.y };
							if (obj.hasFlipFlags(tson::TileFlipFlags::Horizontally))
							{
								scale.x = -scale.x;
							}
							if (obj.hasFlipFlags(tson::TileFlipFlags::Vertically))
							{
								scale.y = -scale.y;
							}
							sprite.setScale(scale);
						}
					}
					sprite.setPosition(position);

					if (IsInsideView(sprite.getPosition(), obj.getSize()))
					{
						_window.draw(sprite, _states);
					}
					else if (obj.getName() == "Background")
					{
						_window.draw(sprite, _states);
					}
				}
			}
			else
			{
				for (auto& m_text : m_textList)
				{
					_window.draw(m_text);
				}
			}
		}
	}
	else
	{
		//std::cerr << "Layer " << _layerName << " not found in map " << _fileName << std::endl;
		return;
	}
}

bool MapManager::IsInsideView(const sf::Vector2f& _pos, tson::Vector2i _objSize)
{
	sf::Vector2f camSize = CameraManager::GetInstance()->GetCameraSize();
	sf::Vector2f cameraPos = CameraManager::GetInstance()->GetView()->getCenter();

	float minX = cameraPos.x - (camSize.x);
	float maxX = cameraPos.x + (camSize.x);
	float minY = cameraPos.y - (camSize.y);
	float maxY = cameraPos.y + (camSize.y);

	if (_pos.x > minX - (float)_objSize.x && _pos.x  < maxX + (float)_objSize.x &&
		_pos.y  > minY - (float)_objSize.y && _pos.y < maxY + (float)_objSize.y)
	{
		return true;
	}
	return false;
}

#pragma endregion

#pragma region MapInfortionCreation
void MapManager::CreateB2HitboxOfLayer(tson::Layer* _childlayer)
{

	if (_childlayer != nullptr)
	{
		//getProperties().getValue<T>("<name>")
		//float value = layer->get()<float>("zIndex");
		for (const auto& obj : _childlayer->getObjects())
		{
			sf::Vector2f objSize = { (float)obj.getSize().x, (float)obj.getSize().y };
			sf::Vector2f localPosition =
			{
				(float)obj.getPosition().x + objSize.x / 2.f + m_mapPosOffset[m_currentMapName].x,
				(float)obj.getPosition().y + objSize.y / 2.f + m_mapPosOffset[m_currentMapName].y
			};

			switch (obj.getObjectType())
			{

			case tson::ObjectType::Rectangle:

				if (_childlayer->getName() == "Pick")
				{
					float bounceFactor = _childlayer->getProperties().getValue<float>("PickBounceFactor");
					objSize = { objSize.x - 64.f, objSize.y - 64.f };

					if (obj.getName() != "")
					{
						//m_b2ShapeList.push_back(new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, obj.getName()));
						//m_b2ShapeList.back()->SetPhysics(1, 1, false, bounceFactor); //NUL
						// 

						m_b2ShapeHashList.emplace(obj.getId(), new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, obj.getName()));
						m_b2ShapeHashList[obj.getId()]->SetPhysics(1, 1, false, bounceFactor); //NUL
					}
					else
					{
						//m_b2ShapeList.push_back(new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, _childlayer->getName()));
						//m_b2ShapeList.back()->SetPhysics(1, 1, false, bounceFactor); //NUL

						m_b2ShapeHashList.emplace(obj.getId(), new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, _childlayer->getName()));
						m_b2ShapeHashList[obj.getId()]->SetPhysics(1, 1, false, bounceFactor); //NUL

					}

				}
				else
				{
					if (obj.getName() != "")
					{
						//m_b2ShapeList.push_back(new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, obj.getName()));

						m_b2ShapeHashList.emplace(obj.getId(), new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, obj.getName()));
					}
					else
					{
						m_b2ShapeHashList.emplace(obj.getId(), new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, _childlayer->getName()));
					}
				}
				break;

			case tson::ObjectType::Polygon:
			{
				b2Vec2* b2points = new b2Vec2[obj.getPolygons().size()];
				for (size_t i = 0; i < obj.getPolygons().size(); i++)
				{
					b2points[i] = b2Vec2(obj.getPolygons()[i].x / MtoP, -obj.getPolygons()[i].y / MtoP);
				}

				if (obj.getName() != "")
				{

					m_b2ShapeHashList.emplace(obj.getId(), new b2::Polygone(*GameData::World, b2_staticBody, localPosition, b2points, (int)obj.getPolygons().size(), obj.getName()));
				}
				else
				{
					m_b2ShapeHashList.emplace(obj.getId(), new b2::Polygone(*GameData::World, b2_staticBody, localPosition, b2points, (int)obj.getPolygons().size(), _childlayer->getName()));
				}
			}
			break;
			case tson::ObjectType::Ellipse:
			{
				float radius = objSize.x / 2;
				sf::Vector2f origin = { objSize.x / 2.f, objSize.y / 2.f };
				localPosition = { localPosition.x - origin.x, localPosition.y - origin.y };

				if (obj.getName() != "")
				{

					m_b2ShapeHashList.emplace(obj.getId(), new b2::Circle(*GameData::World, b2_staticBody, localPosition, radius, origin, obj.getName()));

				}
				else
				{
					m_b2ShapeHashList.emplace(obj.getId(), new b2::Circle(*GameData::World, b2_staticBody, localPosition, radius, origin, _childlayer->getName()));

				}
			}
			break;
			case tson::ObjectType::Object:
			{
				localPosition.y -= objSize.y;
				if (obj.getName() != "")
				{
					m_b2ShapeHashList.emplace(obj.getId(), new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, obj.getName()));
				}
				else
				{
					m_b2ShapeHashList.emplace(obj.getId(), new b2::Rectangle(*GameData::World, b2_staticBody, localPosition, objSize, _childlayer->getName()));
				}
				//Set sensor to true
				m_b2ShapeHashList[obj.getId()]->GetBody()->GetFixtureList()[0].SetSensor(true);
			}
			break;
			}
		}
	}
	else
	{
		return;
	}
}


void MapManager::CreateObjectOfLayer(tson::Layer* _childLayer)
{
	//tson::Layer* layer = m_currentMapData->getLayer(_layerName);

	if (_childLayer != nullptr)
	{
		if (_childLayer->getType() == tson::LayerType::ObjectGroup)
		{
			uint32_t id = 0;
			for (auto& obj : _childLayer->getObjects())
			{
				sf::Vector2f objSize = { (float)obj.getSize().x, (float)obj.getSize().y };
				sf::Vector2f objScale = { 1.f, 1.f };

				switch (obj.getObjectType())
				{
				case tson::ObjectType::Object:
				{
					auto* map = _childLayer->getMap();
					tson::Tileset* tileset = _childLayer->getMap()->getTilesetByGid(obj.getGid());
					const sf::Texture* texture = nullptr;
					id = obj.getGid() - tileset->getFirstgid() + 1;
					tson::Tile* tile = tileset->getTile(id);
					bool hasAnimation = tileset->getTile(id)->getAnimation().any();
					sf::IntRect textureRegion;

					if (tileset->getType() == tson::TilesetType::ImageCollectionTileset)
					{
						// Look up tile via object global id
						uint32_t id = obj.getGid() - tileset->getFirstgid() + 1;
						tson::Tile* tile = tileset->getTile(id);
						std::string name = tile->getImage().filename().string();
						texture = TextureManager::GetTexture(name);
						sf::Vector2f offset = GetTileOffset(obj.getGid(), obj.getSize(), tileset);
						textureRegion = { static_cast<int>(offset.x),
										  static_cast<int>(offset.y),
										  obj.getSize().x,
										  obj.getSize().y };

						objScale.x = (float)obj.getSize().x / (float)textureRegion.width;
						objScale.y = (float)obj.getSize().y / (float)textureRegion.height;

						objSize = { (float)obj.getSize().x / 2, (float)obj.getSize().y / 2 };
					}
					else if (tileset->getType() == tson::TilesetType::ImageTileset)
					{
						texture = TextureManager::GetTexture(tileset->getName() + ".png");
						id = obj.getGid() - tileset->getFirstgid() + 1;
						tson::Tile* tile = tileset->getTile(id);
						sf::Vector2f offset = GetTileOffset(obj.getGid(), obj.getSize(), tileset);
						textureRegion = { static_cast<int>(offset.x),
															static_cast<int>(offset.y),
															obj.getSize().x,
															obj.getSize().y };
						objScale.x = (float)obj.getSize().x / (float)textureRegion.width;
						objScale.y = (float)obj.getSize().y / (float)textureRegion.height;

						objSize = { (float)obj.getSize().x / 2, (float)obj.getSize().y / 2 };

					}

					std::string name = obj.getName();
					sf::Vector2f position =
					{
						(float)obj.getPosition().x,
						(float)obj.getPosition().y
					};
					if (texture != nullptr)
					{
						sf::Sprite sprite(*texture, textureRegion);


						sf::Vector2f scale = { objScale.x, objScale.y };
						sf::Vector2f originalScale = scale;
						float rotation = sprite.getRotation();
						float originalRotation = rotation;
						sf::Vector2f origin{ (objSize.x), (objSize.y) };

						if (obj.hasFlipFlags(tson::TileFlipFlags::Horizontally))
						{
							scale.x = -scale.x;
						}
						if (obj.hasFlipFlags(tson::TileFlipFlags::Vertically))
						{
							scale.y = -scale.y;
						}

						//get object rotation
						float rotationFactor = obj.getRotation();

						//position = { position.x + origin.x , position.y + origin.y };
						sprite.setOrigin(origin);
						sprite.setPosition({ position.x , position.y });
						sprite.setScale(scale);
						sprite.setRotation(rotationFactor);

						//sprite.setScale(originalScale);
						//sprite.setRotation(originalRotation);
						m_spriteAtlasList[tileset->getName()][id] = sprite;
					}
				}
				break;
				}
			}
		}
	}
	else
	{
		return;
	}
}
void MapManager::CreateObjectOfLayer(tson::Layer* _childlayer, std::string _parentLayer)
{
	if (_childlayer != nullptr)
	{
		if (_childlayer->getType() == tson::LayerType::ObjectGroup)
		{
			uint32_t id = 0;
			for (auto& obj : _childlayer->getObjects())
			{
				sf::Vector2f objSize = { (float)obj.getSize().x, (float)obj.getSize().y };
				sf::Vector2f objScale = { 1.f, 1.f };

				switch (obj.getObjectType())
				{
				case tson::ObjectType::Object:
				{
					auto* map = _childlayer->getMap();
					tson::Tileset* tileset = _childlayer->getMap()->getTilesetByGid(obj.getGid());
					const sf::Texture* texture = nullptr;
					id = obj.getGid() - tileset->getFirstgid() + 1;
					tson::Tile* tile = tileset->getTile(id);
					bool hasAnimation = tileset->getTile(id)->getAnimation().any();
					sf::IntRect textureRegion;

					if (tileset->getType() == tson::TilesetType::ImageCollectionTileset)
					{
						// Look up tile via object global id
						uint32_t id = obj.getGid() - tileset->getFirstgid() + 1;
						tson::Tile* tile = tileset->getTile(id);
						std::string name = tile->getImage().filename().string();
						texture = TextureManager::GetTexture(name);
						sf::Vector2f offset = GetTileOffset(obj.getGid(), obj.getSize(), tileset);
						textureRegion = { static_cast<int>(offset.x),
										  static_cast<int>(offset.y),
										  obj.getSize().x,
										  obj.getSize().y };

						objScale.x = (float)obj.getSize().x / (float)textureRegion.width;
						objScale.y = (float)obj.getSize().y / (float)textureRegion.height;

						objSize = { (float)obj.getSize().x / 2, (float)obj.getSize().y / 2 };
					}
					else if (tileset->getType() == tson::TilesetType::ImageTileset)
					{
						texture = TextureManager::GetTexture(tileset->getName() + ".png");
						id = obj.getGid() - tileset->getFirstgid() + 1;
						tson::Tile* tile = tileset->getTile(id);
						sf::Vector2f offset = GetTileOffset(obj.getGid(), obj.getSize(), tileset);
						textureRegion = { static_cast<int>(offset.x),
															static_cast<int>(offset.y),
															obj.getSize().x,
															obj.getSize().y };
						objScale.x = (float)obj.getSize().x / (float)textureRegion.width;
						objScale.y = (float)obj.getSize().y / (float)textureRegion.height;

						objSize = { (float)obj.getSize().x / 2, (float)obj.getSize().y / 2 };

					}

					std::string name = obj.getName();
					sf::Vector2f position =
					{
						(float)obj.getPosition().x,
						(float)obj.getPosition().y
					};
					if (texture != nullptr)
					{
						sf::Sprite sprite(*texture, textureRegion);


						sf::Vector2f scale = { objScale.x, objScale.y };
						sf::Vector2f originalScale = scale;
						float rotation = sprite.getRotation();
						float originalRotation = rotation;
						sf::Vector2f origin{ (objSize.x), (objSize.y) };

						if (obj.hasFlipFlags(tson::TileFlipFlags::Horizontally))
						{
							scale.x = -scale.x;
						}
						if (obj.hasFlipFlags(tson::TileFlipFlags::Vertically))
						{
							scale.y = -scale.x;
						}

						//get object rotation
						float rotationFactor = obj.getRotation();

						//position = { position.x + origin.x , position.y + origin.y };
						sprite.setOrigin(origin);
						sprite.setPosition({ position.x , position.y });
						sprite.setScale(scale);
						sprite.setRotation(rotationFactor);

						//sprite.setScale(originalScale);
						//sprite.setRotation(originalRotation);
						m_spriteAtlasList[tileset->getName()][id] = sprite;

						if (_parentLayer == "Interactibles")
						{
							if (obj.getName() != "")
							{
								m_interactibleList[obj.getId()] = obj.getName();
							}
							else
							{
								m_interactibleList[obj.getId()] = _childlayer->getName();
							}
						}
					}
				}
				break;
				}
			}
		}
	}
	else
	{
		return;
	}
}

void MapManager::CreateTextOfLayer(std::string _layerName)
{
	tson::Layer* layer = m_currentMapData->getLayer(_layerName);
	//Check if layer exist
	if (layer != nullptr)
	{
		if (layer->getType() == tson::LayerType::ObjectGroup)
		{
			for (auto& obj : layer->getObjects())
			{
				sf::Vector2f objSize = { (float)obj.getSize().x, (float)obj.getSize().y };
				sf::Vector2f localPosition = { (float)obj.getPosition().x + objSize.x / 2.f, (float)obj.getPosition().y + objSize.y / 2.f };

				switch (obj.getObjectType())
				{
				case tson::ObjectType::Text:
				{


					std::cout << "Font :" << obj.getText().fontFamily << std::endl;

					sf::Text m_text;
					m_text.setFont(m_font);
					m_text.setPosition
					(
						{
							(float)obj.getPosition().x + m_mapPosOffset[m_currentMapName].x,
							(float)obj.getPosition().y + m_mapPosOffset[m_currentMapName].y
						}
					);
					m_text.setString(obj.getText().text);
					m_text.setCharacterSize(obj.getText().pixelSize);
					m_text.setLetterSpacing(obj.getText().kerning);

					sf::Text::Style isBold = obj.getText().bold ? sf::Text::Bold : sf::Text::Regular;
					sf::Text::Style isItalic = obj.getText().italic ? sf::Text::Italic : sf::Text::Regular;
					sf::Text::Style isUnderlined = obj.getText().underline ? sf::Text::Underlined : sf::Text::Regular;
					sf::Text::Style isStrikeThrough = obj.getText().strikeout ? sf::Text::StrikeThrough : sf::Text::Regular;
					m_text.setStyle(isBold | isItalic | isUnderlined | isStrikeThrough);

					sf::Color textColor = sf::Color((int)obj.getText().color.r, (int)obj.getText().color.g, (int)obj.getText().color.b, (int)obj.getText().color.a);
					m_text.setFillColor(textColor);
					//	m_text.setScale(obj.getText()., 2.0f); //Half scale for making a sharp text.

					m_textList.push_back(m_text);
				}
				break;
				}
			}
		}
	}
	else
	{
		return;
	}
}

void MapManager::LoadEventsInformation(std::string _layerName)
{
	tson::Layer* layer = m_mapHashList[m_currentMapName]->getLayer(_layerName);
	int count = 0;
	//Check if layer exist
	if (layer != nullptr)
	{
		if (layer->getType() == tson::LayerType::Group)
		{
			for (auto& subLayer : layer->getLayers())
			{
				if (subLayer.getName() == "Events")
				{
					for (auto& obj : subLayer.getObjects())
					{
						sf::Vector2f objSize = { (float)obj.getSize().x, (float)obj.getSize().y };
						sf::Vector2f localPosition = { (float)obj.getPosition().x + objSize.x / 2.f, (float)obj.getPosition().y + objSize.y / 2.f };
						sf::Vector2f objPos = { (float)obj.getPosition().x, (float)obj.getPosition().y };
						std::string objName = obj.getName();
						switch (obj.getObjectType())
						{
						case tson::ObjectType::Point:
						{
							sf::Vector2f localPosition =
							{
								(float)objPos.x + m_mapPosOffset[m_currentMapName].x,
								(float)objPos.y + m_mapPosOffset[m_currentMapName].y
							};

							if (objName == "SpawnPlayer")
							{
								m_playerSpawn = localPosition;
							}
							else if (objName == "EnemySpider")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyBee")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyAnguille")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyShrimp")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyOctopus")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyRay")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyBalloon")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyEel")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyFly")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "EnemyMantis")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "Teleporter")
							{
								m_teleporterSpawnist[objName].push_back(localPosition);
							}
							else if (objName == "BossChase")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "Chase_PlayerCinematicSpawn")
							{
								BossChase::PlayerCinematicSpawnBegin = localPosition;
							}
							else if (objName == "Chase_PlayerCinematicSpawn2")
							{
								BossChase::PlayerCinematicSpawnEnd = localPosition;
							}
							else if (objName == "Chase_CinematicCamera1")
							{
								BossChase::CinematicCameraBegin = localPosition;
							}
							else if (objName == "Chase_CinematicCamera2")
							{
								BossChase::CinematicCameraEnd = localPosition;
							}
							else if (objName == "Boss_Spawn")
							{
								m_enemySpawnList[objName].push_back(localPosition);
								Boss::BossSpawnPoint = localPosition;
							}
							else if (objName == "Boss_BeginPoint")
							{
								Boss::BeginPoint = localPosition;
							}
							else if (objName == "Boss_ShockwaveSpawn")
							{
								Boss::ShockwaveSpawnPoint = localPosition;
							}
							else if (objName == "BackgroundSpider")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
							else if (objName == "BackgroundOctopus")
							{
								m_enemySpawnList[objName].push_back(localPosition);
							}
						}
						break;
						}
					}
				}
			}



		}

	}
	else
	{
		return;
	}
}

void MapManager::LoadZonesInformation(std::string _layerName)
{
	tson::Layer* layer = m_mapHashList[m_currentMapName]->getLayer(_layerName);

	if (layer != nullptr)
	{
		if (layer->getType() == tson::LayerType::Group)
		{
			for (auto& subLayer : layer->getLayers())
			{
				if (subLayer.getName() == "ZoneExtra")
				{
					for (auto& obj : subLayer.getObjects())
					{
						sf::Vector2f objSize = { (float)obj.getSize().x, (float)obj.getSize().y };
						sf::Vector2f localPosition = { (float)obj.getPosition().x + objSize.x / 2.f, (float)obj.getPosition().y + objSize.y / 2.f };
						sf::Vector2f objPos = { (float)obj.getPosition().x, (float)obj.getPosition().y };

						std::string objName = obj.getName();
						std::string objClassName = obj.getType();
						sf::Vector2f size = { (float)objSize.x, (float)objSize.y };
						switch (obj.getObjectType())
						{
						case tson::ObjectType::Rectangle:
						{
							sf::Vector2f localPosition =
							{
								(float)objPos.x + m_mapPosOffset[m_currentMapName].x,
								(float)objPos.y + m_mapPosOffset[m_currentMapName].y
							};

							m_zonesMapBounds[m_currentMapName][objName] = { localPosition, size };
						}
						break;
						}
					}
				}
			}

		}
	}
	else
	{
		return;
	}
}



void MapManager::CreateMapTiles(tson::Layer* _layer)
{
	//tson::Layer* layer = m_mapHashList[m_currentMapName]->getLayer(_layerName);

	//Check if layer exist
	if (_layer != nullptr)
	{
		if (_layer->getType() == tson::LayerType::TileLayer)
		{
			for (auto& [pos, tileObject] : _layer->getTileObjects())
			{
				//Set sprite data to draw the tile
				tson::Tileset* tileset = tileObject.getTile()->getTileset();
				bool hasAnimation = tileObject.getTile()->getAnimation().any();
				tson::Rect drawingRect;

				if (!hasAnimation)
				{
					drawingRect = tileObject.getDrawingRect();
				}
				else
				{
					uint32_t ownerId = tileObject.getTile()->getId();
					if (m_animationUpdateQueue.count(ownerId) == 0) //This is only built once to track all tile IDs with animations
					{
						m_animationUpdateQueue[ownerId] = &tileObject.getTile()->getAnimation();
					}
					uint32_t tileId = tileObject.getTile()->getAnimation().getCurrentTileId();
					tson::Tile* animatedTile = tileset->getTile(tileId);
					drawingRect = animatedTile->getDrawingRect();
				}


				const sf::Texture* texture = nullptr;
				if (tileset->getType() == tson::TilesetType::ImageCollectionTileset)
				{
					tson::Tile* tile = tileObject.getTile();
					if (tile->getFlipFlags() != tson::TileFlipFlags::None)
					{
						tile = tileset->getTile(tile->getId());
					}
					texture = TextureManager::GetTexture(tileset->getName() + ".png");

				}
				else if (tileset->getType() == tson::TilesetType::ImageTileset)
				{
					texture = TextureManager::GetTexture(tileset->getName() + ".png");
				}

				if (texture != nullptr)
				{
					sf::Vector2f position{ tileObject.getPosition().x, tileObject.getPosition().y };
					sf::Vector2f positionOffset
					{
						static_cast<float>(m_mapPosOffset[m_currentMapName].x),
							static_cast<float>(m_mapPosOffset[m_currentMapName].y)
					};

					tson::Tile* tile = tileObject.getTile();
					TileSpriteSettings params
					{
						*texture, // texture
						{ drawingRect.x, drawingRect.y, drawingRect.width, drawingRect.height },
						position + positionOffset,
						tile->hasFlipFlags(tson::TileFlipFlags::Horizontally), // hortFlip
						tile->hasFlipFlags(tson::TileFlipFlags::Vertically), // vertFlip
						tile->hasFlipFlags(tson::TileFlipFlags::Diagonally),
						(int)tile->getId(),
						tileset->getName()
					};
					CreateTileSprite(params);
				}
			}
		}
	}
	else
	{
		return;
	}
}
void MapManager::CreateTileSprite(TileSpriteSettings& _settings)
{
	sf::Sprite sprite(_settings.texture, _settings.textureRegion);

	// Center origin so scale and rotation does not displace sprite
	sf::Vector2f originOffset = sf::Vector2f(_settings.textureRegion.width / 2.f, _settings.textureRegion.height / 2.f);
	sprite.setOrigin(originOffset);

	if (_settings.diagonalFlip)
	{
		sprite.rotate(90);

		// Swap horizontal and vertical flips
		bool temp = _settings.hortFlip;
		_settings.hortFlip = _settings.vertFlip;
		_settings.vertFlip = temp;

		// Compensate for the swap of image dimensions
		originOffset += sf::Vector2f(originOffset.y - originOffset.x, originOffset.y - originOffset.x);
	}

	sprite.setScale(_settings.hortFlip ? -1.0f : 1.0f, _settings.vertFlip ? -1.0f : 1.0f);
	sprite.setPosition(_settings.pos + originOffset);

	m_spriteAtlasList[_settings.texName][_settings.tileID] = sprite;
}

sf::Vector2f MapManager::GetTileOffset(int _tileId, tson::Vector2i _objSize, tson::Tileset* _tileset)
{
	uint32_t firstId = _tileset->getFirstgid();
	int columns = _tileset->getColumns();
	int rows = 0;
	if (columns > 0)
	{
		//columns = _tileset->getColumns();
		rows = _tileset->getTileCount() / columns;
	}
	else
	{
		//columns = _tileset->getTileCount();
		rows = _tileset->getTileCount();
	}
	//int rows = _tileset->getTileCount() / columns;
	uint32_t lastId = (_tileset->getFirstgid() + _tileset->getTileCount()) - 1;

	if ((unsigned)_tileId >= firstId && (unsigned)_tileId <= lastId)
	{
		uint32_t baseTilePosition = ((unsigned)_tileId - firstId);

		int tileModX = 0;
		int currentRow = 0;
		if (columns > 0)
		{
			tileModX = (baseTilePosition % columns);
			currentRow = (baseTilePosition / columns);
		}
		else
		{
			tileModX = columns;
			currentRow = 0;
		}

		int offsetX = (tileModX != 0) ? ((tileModX)*_objSize.x) : (0 * _objSize.x);
		int offsetY = (currentRow < rows) ? (currentRow * _objSize.y) : ((rows)*_objSize.y);
		return sf::Vector2f((float)offsetX, (float)offsetY);
	}

	return { 0.f, 0.f };
}
#pragma endregion

void MapManager::DeleteB2Hitbox()
{
	//for (auto& shape : m_b2ShapeList)
	//{
	//	delete shape;
	//	shape = nullptr;
	//}
	//m_b2ShapeList.clear();
	//m_b2ShapeList.shrink_to_fit();

	for (auto& shape : m_b2ShapeHashList)
	{
		delete shape.second;
		shape.second = nullptr;
	}

	m_b2ShapeHashList.clear();
}



