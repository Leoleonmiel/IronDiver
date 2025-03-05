#include "MiniMapManager.h"
#include "Entity.h"
#include "CharacterController.h"
#include "common.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "EntityManager.h"
#include "TextureManager.h"
#include "WorldManager.hpp"

#define MAP_SPEED 500
#define ZOOM_POWER 2

MiniMapManager* MiniMapManager::Instance = nullptr;

void AdjustSprite(sf::Vector2f _pos, sf::Color _color, sf::Vector2f _scale, sf::Sprite& _sprite)
{
	sf::Image tempImage;
	sf::Texture* tempTexture = new sf::Texture();

	tempImage.create((unsigned)_scale.x, (unsigned)_scale.y, _color);
	tempTexture->loadFromImage(tempImage);
	_sprite.setTexture(*tempTexture);

	// Center it
	sf::Vector2u tempTaille = tempTexture->getSize();
	sf::Vector2f tempOrigin = sf::Vector2f(tempTaille.x / 2.f, tempTaille.y / 2.f);
	_sprite.setOrigin(tempOrigin);
}

MiniMapManager::~MiniMapManager()
{
	delete m_mapView;
	m_mapView = nullptr;
	delete m_playerSprite;
	m_playerSprite = nullptr;
	delete m_mapSprite;
	m_mapSprite = nullptr;
}

MiniMapManager* MiniMapManager::GetInstance()
{
	if (Instance == nullptr) 
	{
		Instance = new MiniMapManager();
	}
	return Instance;
}

void MiniMapManager::Init()
{
	m_mapView = new sf::View();
	m_playerSprite = TextureManager::CreateSprite("playerPoint.png", true);
	m_mapSprite = TextureManager::CreateSprite("MAP.png");
	m_mapBackground = TextureManager::CreateSprite("Ecran_MAP.png");
	b2Vec2 pos = Screen2World(WorldManager::GetInstance()->GetPlayerWorldPos() - sf::Vector2f{ 0, 50.f });
	m_mapSprite->setPosition(pos.x, pos.y);
	m_mapSprite->scale(1.57f,1.6f);
	m_mapBackground->setPosition(pos.x, pos.y);

	// Center it
	sf::Vector2u tempTaille = m_mapBackground->getTexture()->getSize();
	sf::Vector2f tempOrigin = sf::Vector2f(tempTaille.x / 2.f, tempTaille.y / 2.f);
	m_mapBackground->setOrigin(tempOrigin);
}

void MiniMapManager::Update()
{
	//Update positions
	m_playerSprite->setPosition(GameData::CharaCtrl->GetPosB2().x + OFFSET.x, GameData::CharaCtrl->GetPosB2().y * -1 + OFFSET.y);

	// Move around in the map
	if (m_isFullMap) 
	{
		InputManager& inputs = *InputManager::GetInstance();
		const float deltaTime = GameData::DeltaTime;

		//Move the minimap camera
		if (inputs.IsLongPress("MoveUp"))
		{
			m_mapView->move(0, -MAP_SPEED * deltaTime);
		}
		if (inputs.IsLongPress("MoveDown"))
		{
			m_mapView->move(0, MAP_SPEED * deltaTime);
		}
		if (inputs.IsLongPress("MoveRight"))
		{
			m_mapView->move(MAP_SPEED * deltaTime, 0);
		}
		if (inputs.IsLongPress("MoveLeft"))
		{
			m_mapView->move(-MAP_SPEED * deltaTime, 0);
		}
		if (inputs.IsSinglePress("Attack")) 
		{
			m_oldMousePos = GameData::GetMousePos();
		}
		if (inputs.IsLongPress("Attack")) 
		{
			if (m_oldMousePos != GameData::GetMousePos()) 
			{
				m_mapView->move((GameData::GetMousePos().x - m_oldMousePos.x) * -1, (GameData::GetMousePos().y - m_oldMousePos.y) * -1);
			}
			m_oldMousePos = GameData::GetMousePos();
		}
		// Zoom map
		if(inputs.IsSinglePress("Wheel"))
		{
			float value = inputs.GetMouseWheelDelta();
			if(value < 0) m_mapView->zoom(1.0f+ ZOOM_POWER * deltaTime);
			else if (value > 0) m_mapView->zoom(1.0f - ZOOM_POWER * deltaTime);
		}
		// quit map
		if (inputs.IsSinglePress("Escape")) 
		{
			SetFullMap(false);
		}
	}
}

void MiniMapManager::Display(sf::RenderWindow& _window)
{
	if (m_isFullMap) 
	{
		CameraManager* camera = CameraManager::GetInstance();
		sf::View tempview = _window.getView();
		_window.setView(*m_mapView);

		sf::RenderTexture renderTexture;
		renderTexture.create((unsigned)m_mapView->getSize().x, (unsigned)m_mapView->getSize().y);
		renderTexture.clear(sf::Color::Black);
		renderTexture.setView(*m_mapView);

		// Create the sprite holding the RenderTexture at the center of the screen
		sf::Sprite* tempSprite = new sf::Sprite();
		tempSprite->setTexture(renderTexture.getTexture());
		sf::Vector2u tempTaille = tempSprite->getTexture()->getSize();
		sf::Vector2f tempOrigin = sf::Vector2f(tempTaille.x / 2.f, tempTaille.y / 2.f);
		tempSprite->setOrigin(tempOrigin);
		tempSprite->setPosition(_window.getView().getCenter());

		//Resize background to correspond to the camera size
		m_mapBackground->setScale(m_mapView->getSize().x / m_mapBackground->getTexture()->getSize().x, m_mapView->getSize().y / m_mapBackground->getTexture()->getSize().y);
		m_mapBackground->setPosition(m_mapView->getCenter());

		//Draw on the RenderTexture, then on the window
		renderTexture.draw(*m_mapBackground);
		renderTexture.draw(*m_mapSprite);
		renderTexture.draw(*m_playerSprite);

		renderTexture.display();

		_window.draw(*tempSprite);
		_window.setView(tempview);
	}
}

void MiniMapManager::SetFullMap(bool _value)
{
	if (_value == true) 
	{
		//Stop all in game movements (player & ennemies & items)
		GameData::CharaCtrl->SetState(GameData::CharaCtrl->INVULNERABLE);
		GameData::GameSpeed = 0.f;
		EntityManager::GetInstance()->SetGameSpeedGoal(0.f);
		EntityManager::GetInstance()->SetGameSpeed(0.f);
		m_mapView->setCenter(m_playerSprite->getPosition());
		//m_mapView->setViewport(GameData::Window->getView().getViewport());
		m_mapView->setSize(sf::Vector2f(SCREEN_SIZE.x * 0.5f, SCREEN_SIZE.y * 0.5f));
	}
	else if(_value == false && m_isFullMap == true)
	{
		//Start all in game movements
		GameData::CharaCtrl->SetState(GameData::CharaCtrl->WAIT);
		GameData::GameSpeed = 1.f;
		EntityManager::GetInstance()->SetGameSpeedGoal(1.f);
		EntityManager::GetInstance()->SetGameSpeed(1.f);
	}
	m_isFullMap = _value;
}

void MiniMapManager::FreeMemory()
{
	delete Instance;
	Instance = nullptr;
}
