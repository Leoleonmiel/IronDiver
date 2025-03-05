#include "Animation.h"
#include "Animator.h"
#include "Box2D_World.h"
#include "CharacterController.h"
#include "CharacterStats.h"
#include "common.h"
#include "CutsceneInstructionsInclude.h"
#include "Cutscenes.h"
#include "DialogModule.h"
#include "GUI.h"
#include "Inventory.h"
#include "StandardParticles.h"
#include "GrabHook.h"
#include "MainMenu.h"
#include "Tileson/tileson.hpp"
#include "Cursor.h"
#include "SaveManager.h"
#include "EntityInclude.h"
#include "ManagerInclude.h"
#include "GameStateInclude.h"
#include "BossRise.h"
#include "BossChaseState.h"
#include "BossChaseEnd.h"
#include "ParticleEnvironement.h"
#include "ParticleSystem.h"
#include <random>
#include "Slider.h"
#include "ShadersManager.hpp"

#define WorldManager WorldManager::GetInstance()

bool MineState::m_firstDialog = false;
bool MineState::m_hookGot = false;
bool MineState::m_attackGot = false;

bool MineState::NearTeleporter = false;
sf::Vector2f MineState::CloseTeleporterPos = sf::Vector2f(0, 0);
ParticleSystem* MineState::ExplosionSystem = nullptr;
ParticleEmitter* MineState::ExplosionEmitter = nullptr;
sf::RectangleShape* MineState::m_cinematicFrames = nullptr;


void InitTeleportAnimation(Animator*& _animator, Animation*& _animation, sf::Sprite*& _sprite)
{

	GetPositionFunc* func = new GetPositionFunc;
	*func = std::bind(&CharaControl::GetPos, GameData::CharaCtrl, sf::Vector2f(0, 0));
	_animator = new Animator(func);

	_sprite = new sf::Sprite();
	_sprite->setOrigin(150, 500);
	sf::Texture* animTpTexture = TextureManager::GetTexture("TpAnim.png");
	_animation = new Animation(CreateAnimation(*_sprite, *animTpTexture, 9, 12.f, { 0,0,300,600 }));
	_animation->loop = false;
}

//#define TiledManager TiledManager::GetInstance()
MineState::MineState()
{
	GameData::World = new b2::World(GRAVITY_BOX2D, *GameData::Window);

	InitManagers();
	InitCharaCtrl();
	InitCamera();
	InitEntities();
	InitSound();

	GameData::Collectables = &m_collectables;

	m_lifeBarSprites[0] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_01.png", true));
	m_lifeBarSprites[1] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_02.png", true));
	m_lifeBarSprites[2] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_03.png", true));
	m_lifeBarSprites[3] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_04.png", true));
	m_lifeBarSprites[4] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_05.png", true));
	m_lifeBarSprites[5] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_06.png", true));
	m_lifeBarSprites[6] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_07.png", true));
	m_lifeBarSprites[7] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_08.png", true));
	m_lifeBarSprites[8] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_09.png", true));
	m_lifeBarSprites[9] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_10.png", true));
	m_lifeBarSprites[10] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HPBar/Vie_11.png", true));

	m_lifeBarRedSprites[0] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/ALL RED/Vie_01.png", true));
	m_lifeBarRedSprites[1] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/ALL RED/Vie_02.png", true));
	m_lifeBarRedSprites[2] = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/ALL RED/Vie_03.png", true));

	m_hudHPSprite = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/hp_bar.png", true));
	m_hudHPSprite->setPosition(lifeBarHUDOriginalPos);

	m_hudRedHPSprite = new sf::Sprite(LoadSprite("Assets/Sprites/HUD/HUD_HpRed.png", true));
	m_hudRedHPSprite->setPosition(lifeBarHUDOriginalPos);

	/*lifeBar = new sf::RectangleShape(fullLifeSize);
	lifeBar->setFillColor(sf::Color(48, 136, 197));
	lifeBar->setPosition(lifeBarOriginalPos);*/

	m_particleEnvironment = new ParticleEnvironment;
	//m_shadersManager = new ShadersManager;
}

void MineState::Init()
{
	Boss::BossSpawned = false;
	Boss::BossBeaten = false;

	InitGUI();
	InitDialogs();
	InitTeleportAnimation(m_tpAnimator, m_tpAnim, m_tpAnimSprite);

	Inventory::playerInventory = new PlayerInventory();

	auto teleporters = WorldManager->GetInstance()->GetTeleporterSpawnList();

	BossChase::RespawnPoint = teleporters["Teleporter"][1];
	BossChase::EndPoint = teleporters["Teleporter"][2] - sf::Vector2f{ 2500, 0 };
	GameData::Cursor->SetIsVisible(true);
	if (SaveManager::loadAfterInit) SaveManager::Load();
	m_camera->SetCameraFixed(false);

}

void MineState::Quit()
{
	WorldManager->ResetWorld();
	delete m_charaCtrl;
	delete m_healthAmount;
	delete m_particleEnvironment;
	m_entityManager->Clear();
	MiniMapManager::FreeMemory();
	ParticleManager::Clear();

	if (ExplosionSystem != nullptr)
	{
		delete ExplosionSystem;
		delete ExplosionEmitter;
		ExplosionSystem = nullptr;
		ExplosionEmitter = nullptr;
	}

	for (auto& collect : m_collectables)
	{
		delete collect;
	}
	m_collectables.clear();
	delete GameData::World->GetWorld();
	GameData::Cursor->SetIsVisible(false);
	CameraManager::Shake(0.f);
	GameData::GameSpeed = 1.f;
	//FxManager::GetInstance()->Clear(); <- This crash the game

}

void MineState::Update(float _dt)
{
	////////////////////////////////////////////////
	//don't place any functions before world update
	GameData::World->Update();

	m_particleEnvironment->Update(_dt);
	if (ExplosionSystem != nullptr)
	{
		//sf::IntRect bossArena = sf::IntRect{ { Boss::BossArena.left, Boss::BossArena.top }, { Boss::BossArena.left + Boss::BossArena.width, Boss::BossArena.top + Boss::BossArena.height } };
		//sf::IntRect bossBox = sf::IntRect{ { m_boss->GetScreenPos().x - 200, m_boss->GetScreenPos().y - 200}, {m_boss->GetScreenPos().x + 200, m_boss->GetScreenPos().y + 200} };


		ExplosionEmitter->SetRandomPosition({ m_boss->GetScreenPos().x - 200, m_boss->GetScreenPos().y - 200 }, { m_boss->GetScreenPos().x + 200, m_boss->GetScreenPos().y + 200 });
		ExplosionSystem->Update(GameData::DeltaTime * 0.5f);
	}

	NearTeleporter = false;

	TimeSpeedManager::Update();

	InputManager& inputs = *InputManager::GetInstance();

	UpdateCurrentMapPlayer();
	WorldManager->Update(_dt);
	//ParticleManager::UpdateParticles();
	if (DialogMaster::DialogActive())
	{
		if (inputs.IsSinglePress("Interact"))
		{
			DialogMaster::PassDialog();
		}
	}

	UpdateTeleporters();

	SoundManager::GetInstance()->Update();
	SoundManager::GetInstance()->SetVolumeAccordingToValue("WindSpeed", sqrt(DistanceSquared(m_charaCtrl->GetGrapHook()->GetShooterShape()->GetVelocity(), { 0,0 })), 50.f);
	m_charaCtrl->Update();
	m_camera->Update();
	CutsceneMaster::Update();
	ScrollManager::GetInstance()->Update();
	Inventory::playerInventory->Update();

	m_isPlayerSpawned = false;

	// life decrease
	m_timer += _dt;

	if (m_timer >= 10.0f)
	{
		float currentHealth = (float)m_charaCtrl->GetStats()->GetHp();
		currentHealth += -1.f * GameData::GameSpeed;

		if (currentHealth < 0.0f) {
			currentHealth = 0.0f;
		}

		m_charaCtrl->GetStats()->SetHp((int)currentHealth);
		m_timer = 0;
	}
	if (inputs.IsSinglePress("Escape") && !MiniMapManager::GetInstance()->IsFullMap())
	{
		if (!m_isGamePaused)
		{
			m_isGamePaused = true;
			m_charaCtrl->SetCanControl(false);
			m_lastGameSpeed = GameData::GameSpeed;
			GameData::GameSpeed = 0;
			GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");
			pauseGUI->SetEnabled(true);
		}
		else
		{
			m_isGamePaused = false;
			m_charaCtrl->SetCanControl(true);
			GameData::GameSpeed = m_lastGameSpeed;
			GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");
			pauseGUI->SetEnabled(false);
		}
	}
	MiniMapManager::GetInstance()->Update();
	if (inputs.IsSinglePress("SpawnItem"))
	{
		m_collectables.push_back(new Collectable(EntityDef{ 0, false, false, 0, 0 }, ItemManager::GetItem("Rock"), GameData::GetRelativeMousePos()));
	}
	if (inputs.IsSinglePress("Map") && !Inventory::playerInventory->showingInventory)
	{
		MiniMapManager::GetInstance()->SetFullMap(!MiniMapManager::GetInstance()->IsFullMap());
	}

	if (m_isGamePaused)
	{
		sf::Vector2i mousePos = (sf::Vector2i)GameData::GetMousePos();

		GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");
		GUI* settingsGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("settingsMenu");

		GUIText* tempText = nullptr;
		if (!settingsGUI->IsEnabled())
		{
			GUI* playButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PlayButton");
			GUI* backMenuButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("BackMenuButton");
			GUI* saveButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("SaveButton");
			GUI* loadButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("LoadButton");
			GUI* settingsButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("settingsButton");

			tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("PlayText");
			tempText->Update(_dt, *GameData::Window);
			tempText->SetInMiddle(*playButton->GetSfShape());
			tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("SaveText");
			tempText->Update(_dt, *GameData::Window);
			tempText->SetInMiddle(*backMenuButton->GetSfShape());
			tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("textSettings");
			tempText->Update(_dt, *GameData::Window);
			tempText->SetInMiddle(*saveButton->GetSfShape());
			tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("LoadText");
			tempText->Update(_dt, *GameData::Window);
			tempText->SetInMiddle(*loadButton->GetSfShape());
			tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("BackMenuText");
			tempText->Update(_dt, *GameData::Window);
			tempText->SetInMiddle(*settingsButton->GetSfShape());
			if (inputs.IsSinglePress("Attack"))
			{
				if (playButton->DetectClick(mousePos, playButton))
				{
					m_isGamePaused = false;
					GameData::GameSpeed = m_lastGameSpeed;
					m_charaCtrl->SetCanControl(true);
					pauseGUI->SetEnabled(false);
				}
				else if (saveButton->DetectClick(mousePos, saveButton))
				{
					m_isGamePaused = false;
					GameData::GameSpeed = m_lastGameSpeed;
					m_charaCtrl->SetCanControl(true);
					pauseGUI->SetEnabled(false);
					SaveManager::Save();
				}
				else if (backMenuButton->DetectClick(mousePos, backMenuButton))
				{
					m_isGamePaused = false;
					GameData::GameSpeed = m_lastGameSpeed;
					m_charaCtrl->SetCanControl(true);
					pauseGUI->SetEnabled(false);
					SaveManager::Save();
					LoadingState::StartLoading<MainMenu>();
					PushState(new FadeOutState(new LoadingState()));
				}
				else if (loadButton->DetectClick(mousePos, loadButton))
				{
					if (SaveManager::CanLoad())
					{
						m_isGamePaused = false;
						GameData::GameSpeed = m_lastGameSpeed;
						pauseGUI->SetEnabled(false);
						m_charaCtrl->SetCanControl(true);
						SaveManager::Load();
						m_camera->SetCharacter(m_charaCtrl);
					}
				}
				else if (settingsButton->DetectClick(mousePos, settingsButton))
				{
					settingsGUI->SetEnabled(true);
					pauseGUI->SetEnabled(false);
				}
			}
		}
		else
		{
			SoundManager::GetInstance()->SetVolume(m_volumeSlider->getSliderValue());

			GUI* backButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("BackButton");
			GUI* fullscreenToggle = (GUI*)GUIManager::GetInstance()->GetNodeByTag("Fullscre");
			GUI* fullscreenCheckbox = (GUI*)GUIManager::GetInstance()->GetNodeByTag("fullScreenToggle");

			tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("BackText");
			tempText->Update(_dt, *GameData::Window);
			tempText->SetInMiddle(*backButton->GetSfShape());
			tempText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("fullText");
			tempText->Update(_dt, *GameData::Window);
			tempText->SetInMiddle(*fullscreenToggle->GetSfShape());
			m_volumeSlider->Update();

			if (inputs.IsSinglePress("Attack"))
			{
				if (backButton->DetectClick((sf::Vector2i)GameData::GetMousePos(), backButton))
				{
					settingsGUI->SetEnabled(false);
					pauseGUI->SetEnabled(true);
				}
				if (fullscreenToggle->DetectClick((sf::Vector2i)GameData::GetMousePos(), fullscreenToggle) || fullscreenCheckbox->DetectClick((sf::Vector2i)GameData::GetMousePos(), fullscreenCheckbox))
				{
					GameData::fullscreen = !GameData::fullscreen;
					if (GameData::fullscreen)
					{
						GUIManager::GetInstance()->GetNodeByTag("togglePoint")->SetEnabled(true);
						GameData::Window->create(sf::VideoMode::getDesktopMode(), "IronDiver", sf::Style::Fullscreen);
					}
					else
					{
						GUIManager::GetInstance()->GetNodeByTag("togglePoint")->SetEnabled(false);
						GameData::Window->create(sf::VideoMode::getDesktopMode(), "IronDiver", sf::Style::Default);
					}
					GameData::Window->setFramerateLimit(MAX_FPS);
					GameData::Window->setVerticalSyncEnabled(true);
					GameData::Window->setMouseCursorVisible(false);
				}
			}
			std::ofstream optionFile("ConfigFiles/Options.txt");
			optionFile << "fullscreen " << GameData::fullscreen << std::endl;
			optionFile << "volume " << m_volumeSlider->getSliderValue() << std::endl;
			optionFile.close();
		}
	}

	for (auto it = m_collectables.begin(); it != m_collectables.end();)
	{
		auto collectable = *it;
		if (collectable->GetIsInInventory())
		{
			it = m_collectables.erase(it);
		}
		else
		{
			collectable->Update(m_charaCtrl->GetPos(), Inventory::playerInventory);
			++it;
		}
	}

	if (inputs.IsSinglePress("Inventory") && !MiniMapManager::GetInstance()->IsFullMap())
	{
		Inventory::playerInventory->showingInventory = !Inventory::playerInventory->showingInventory;
		if (!Inventory::playerInventory->showingInventory)
		{
			m_entityManager->SetGameSpeedGoal(1);
			m_entityManager->SetGameSpeed(1);
		}
		else
		{
			m_entityManager->SetGameSpeedGoal(0);
			m_entityManager->SetGameSpeed(0);
		}

	}

	/*float rectangleWidth = 330 * (m_charaCtrl->GetStats()->GetHp() / (float)m_charaCtrl->GetStats()->GetMaxHp());
	lifeBar->setSize(sf::Vector2f(rectangleWidth, 32));*/

	m_entityManager->Update();

	if (m_bossLifeBar != nullptr)
	{
		int health = 0;
		float percent = 0.f;
		if (m_boss != nullptr)
		{
			health = m_boss->GetHealth();
		}

		percent = health / (float)m_bossMaxHealth;
		percent *= 0.6f;
		m_bossLifeBar->setSize({ percent * SCREEN_SIZE.x, 35 });
	}

	m_lifePoints->setString(std::to_string(m_charaCtrl->GetStats()->GetHp())); // Définition du texte

	//m_healthAmount->setString(std::to_string(m_characterStats->GetHealth()));
	//m_healthAmount->setPosition(GameData::Window->getView().getCenter().x - 870, GameData::Window->getView().getCenter().y - 415);

	ParticleManager::UpdateParticles();

	GameData::World->Clear();
	m_entityManager->ExecuteMarkedForDeath();
	//don't place any functions after world cleanup
	///////////////////////////////////////////////
}



void MineState::Display(sf::RenderWindow& _window)
{
	//m_shadersManager->GetMainRT()->clear(sf::Color::Black);

	DialogMaster::UpdateDialog();
	////////////////////////////////////////////////

	WorldManager->DisplayMap(_window);
	m_entityManager->Display();

	if (!m_teleporting || m_tpAnimator->GetCurrentFrame() < 4)
	{
		m_charaCtrl->Display(_window);
	}
	ParticleManager::RenderParticles(_window);
	if (ExplosionSystem != nullptr)
	{
		ExplosionSystem->Display(_window);
	}

	m_tpAnimator->Display(_window);


	WorldManager->DisplayFront(_window);

	//_window.draw(*m_shadersManager->GetMainSceneSprite());
	GameData::World->Display(_window);
#ifdef _DEBUG
	//GameData::World->DisplayDebug({ 0,0 });
#endif


	for (auto& collectable : m_collectables)
	{
		collectable->Display(_window);
	}

	GUIImage* grab = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("Grab");
	GUIImage* jump = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("Jump");

	//std::cout << m_charaCtrl->GetGrabhookIsFullCooldown()<< std::endl;

	if (!m_charaCtrl->GetGrabhookIsFullCooldown())
	{
		grab->SetColor(sf::Color(128, 128, 128));
	}
	else
	{
		grab->SetColor(sf::Color(255, 255, 255));
	}

	if (!m_charaCtrl->GetAttackIsFullCooldown())
	{
		jump->SetColor(sf::Color(128, 128, 128));
	}
	else
	{
		jump->SetColor(sf::Color(255, 255, 255));
	}

	MiniMapManager::GetInstance()->Display(_window);

	GUIText* deepLabel = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("DeepLabel");
	GUIText* kgLabel = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("PodsLabel");
	deepLabel->SetText(std::to_string((int)(-m_charaCtrl->GetPos().y / MtoP)) + "m");
	GUIText* deepLabelRed = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("DeepLabelRed");
	deepLabelRed->SetText(std::to_string((int)(-m_charaCtrl->GetPos().y / MtoP)) + "m");
	kgLabel->SetText(std::to_string(Inventory::playerInventory->GetTotalWeight()));
	if (NearTeleporter)
	{
		m_interactButtonBackground->setPosition(CloseTeleporterPos);
		m_interactButtonText->setPosition(CloseTeleporterPos);

		_window.draw(*m_interactButtonBackground);
		_window.draw(*m_interactButtonText);
	}

	//HUD Display
	sf::View view = _window.getView();
	_window.setView(_window.getDefaultView());

	if (m_guiManager != nullptr && m_guiManager->GetCurrentRoot() != nullptr && m_displayHUD)
	{
		/*_window.draw(*lifeBar);*/
		m_guiManager->GetCurrentRoot()->Display(_window);
		/*GUI* HUD = m_guiManager->GetNodeByTag("BackgroundOutline");
		HUD->Display(_window);*/
		 if(m_guiManager->GetNodeByTag("settingsMenu")->IsEnabled())m_volumeSlider->draw(_window);
	}
	Inventory::playerInventory->ShowInventory(_window);


	if (m_bossLifeBar != nullptr)
	{
		_window.draw(*m_bossLifeBar);
	}

	GUI* hud = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("HUD");
	GUI* hudRed = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("HUDRed");

	sf::Sprite* currentSprite = m_lifeBarSprites[0];

	m_elapsedTimeLifeBar += GameData::DeltaTime;



	if (m_charaCtrl->GetStats()->GetHp() > 45)
	{
		currentSprite = m_lifeBarSprites[0];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 40)
	{
		currentSprite = m_lifeBarSprites[1];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 35)
	{
		currentSprite = m_lifeBarSprites[2];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 30)
	{
		currentSprite = m_lifeBarSprites[3];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 25)
	{
		currentSprite = m_lifeBarSprites[4];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 20)
	{
		currentSprite = m_lifeBarSprites[5];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 15)
	{
		currentSprite = m_lifeBarSprites[6];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 10)
	{
		currentSprite = m_lifeBarSprites[7];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 5)
	{
		currentSprite = m_lifeBarRedSprites[2];
	}
	else if (m_charaCtrl->GetStats()->GetHp() > 0)
	{
		currentSprite = m_lifeBarRedSprites[1];
	}
	else
	{
		currentSprite = m_lifeBarRedSprites[0];
	}

	currentSprite->setPosition(lifeBarHUDOriginalPos);

	if (m_charaCtrl->GetStats()->GetHp() < 11)
	{
		m_elapsedTimeLifeBar += GameData::DeltaTime;

		if (m_elapsedTimeLifeBar < 0.8f)
		{
			std::mt19937 generator((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());
			std::uniform_real_distribution<float> distribution(-3.f, 3.f);

			float offsetX = distribution(generator);
			float offsetY = distribution(generator);

			currentSprite->setPosition(currentSprite->getPosition().x + offsetX, currentSprite->getPosition().y + offsetY);
			//m_lifePoints->setPosition(m_lifePoints->getPosition().x + offsetX, m_lifePoints->getPosition().y + offsetY);

			hudRed->SetEnabled(true);
			hud->SetEnabled(false);
		}
		else
		{
			hudRed->SetEnabled(false);
			hud->SetEnabled(true);

			if (m_charaCtrl->GetStats()->GetHp() > 5)
			{
				currentSprite = m_lifeBarSprites[8];
			}
			else if (m_charaCtrl->GetStats()->GetHp() > 0)
			{
				currentSprite = m_lifeBarSprites[9];
			}
			else
			{
				currentSprite = m_lifeBarSprites[10];
			}

			currentSprite->setPosition(lifeBarHUDOriginalPos);
		}

		m_lifePoints->setPosition(lifePointOriginalPos.x + 10, lifePointOriginalPos.y);

		if (m_elapsedTimeLifeBar > 5.0f)
			m_elapsedTimeLifeBar = 0;
	}


	BlitSprite(*currentSprite, currentSprite->getPosition(), 0, _window);
	_window.draw(*m_lifePoints);

	//else
	//{
	//	m_hudRedHPSprite->setPosition(lifeBarHUDOriginalPos);
	//	lifeBar->setPosition(lifeBarOriginalPos);
	//	hudRed->SetEnabled(false);
	//	hud->SetEnabled(true);
	//	lifeBar->setFillColor(sf::Color(48, 136, 197));
	//	BlitSprite(*m_hudHPSprite, sf::Vector2f(lifeBarHUDOriginalPos), 0, _window);
	//}
	_window.draw(m_cinematicFrames[0]);
	_window.draw(m_cinematicFrames[1]);

	_window.setView(view);

	m_particleEnvironment->Display(_window);

	UpdateCameraWorldZoom();
}

void MineState::HandleEvents(sf::Event& _event)
{

	if (_event.type == sf::Event::KeyPressed)
	{

		if (_event.key.code == sf::Keyboard::R)
		{

			auto teleporters = WorldManager->GetInstance()->GetTeleporterSpawnList();
			sf::Vector2f tpPos;
			tpPos = teleporters["Teleporter"][m_currentTeleporter];
			m_currentTeleporter++;
			if (m_currentTeleporter >= teleporters["Teleporter"].size())
			{
				m_currentTeleporter = 0;
			}

			m_charaCtrl->SetCanAttackSide(true);
			m_charaCtrl->SetCanAttackUp(true);
			m_charaCtrl->SetCanAttackDown(true);
			m_charaCtrl->SetCanShootGrapple(true);

			m_charaCtrl->SetPos(tpPos);
			m_camera->SetCharacter(m_charaCtrl);
			m_hookGot = true;
			m_attackGot = true;
			m_camera->SetCameraZoneFixedView(false);
		}
	}
}

void MineState::UpdateTeleporters()
{
	auto teleporters = WorldManager->GetInstance()->GetTeleporterSpawnList();
	sf::Vector2f playerPos = m_charaCtrl->GetPos();
	sf::Vector2f tpPos;
	sf::Vector2f path;

	for (auto pair : teleporters)
	{
		if (pair.first == "Teleporter")
		{
			for (auto pos : pair.second)
			{
				tpPos = pos;
				path = tpPos - playerPos;
				if (DistanceSquared(path, { 0,0 }) < 100 * 100)
				{
					NearTeleporter = true;
					CloseTeleporterPos = tpPos + sf::Vector2f(0, -100);
					InputManager& inputs = *InputManager::GetInstance();
					if (m_charaCtrl->GetIsFeetColide() && inputs.IsSinglePress("Interact") && DialogMaster::DialogActive() == false)
					{
						std::queue<Animation*> animationQueue;
						animationQueue.push(m_tpAnim);
						m_tpAnimator->SetAnimationQueue(animationQueue);

						LoadingState::StartLoading<ShopState>();
						Inventory::shopInventory->AddPlayerInventoryItems(*Inventory::playerInventory);
						PushState(new FadeOutState(new LoadingState()));
						m_teleporting = true;
					}
				}
			}
		}
	}
}

void MineState::InitEntities()
{
	std::unordered_map<std::string, std::vector<sf::Vector2f>> enemySpawnList = WorldManager->GetEnemySpawnList();

	auto bossChase = enemySpawnList.find("BossChase");
	if (bossChase != enemySpawnList.end())
	{
		m_bossSpawnLocation = enemySpawnList["BossChase"][0];
	}

	bossChase = enemySpawnList.find("Boss_Spawn");
	if (bossChase != enemySpawnList.end())
	{
		Boss::BossSpawnPoint = enemySpawnList["Boss_Spawn"][0];
	}

	for (const auto& [name, positions] : enemySpawnList)
	{
		if (name == "BackgroundSpider")
		{
			for (auto& position : positions)
			{
				m_entityManager->Add(new SpiderEntity(position, true), true);
			}
		}
		else if (name == "BackgroundOctopus")
		{
			for (auto& position : positions)
			{
				m_entityManager->Add(new OctopusEntity(position, true), true);
			}
		}
	}

	//Function pointer list to create entities
	std::unordered_map<std::string, std::function<Entity* (const sf::Vector2f&)>> entityList =
	{
	   {"EnemyOctopus", [](const sf::Vector2f& pos) { return new OctopusEntity(pos);   }},
	   {"EnemySpider" , [](const sf::Vector2f& pos) { return new SpiderEntity(pos);    }},
	   {"EnemyBalloon", [](const sf::Vector2f& pos) { return new JellyfishEntity(pos); }},
	   {"EnemyShrimp" , [](const sf::Vector2f& pos) { return new ShrimpEntity(pos);    }},
	   {"EnemyRay"	  , [](const sf::Vector2f& pos) { return new RayEntity(pos);	   }},
	   {"EnemyEel"	  , [](const sf::Vector2f& pos) { return new EelEntity(pos);	   }},
	   {"EnemyFly"	  , [](const sf::Vector2f& pos) { return new FlyEntity(pos);	   }},
	   {"EnemyMantis" , [](const sf::Vector2f& pos) { return new MantisEntity(pos);	   }}
	};

	for (const auto& pair : enemySpawnList)
	{
		const std::string& enemyType = pair.first;
		const auto& spawnPositions = pair.second;

		auto entityIt = entityList.find(enemyType);
		if (entityIt != entityList.end())
		{
			for (const auto& pos : spawnPositions)
			{
				m_entityManager->Add(entityIt->second(pos));
			}
		}
	}
}

void MineState::UpdateCameraWorldZoom()
{
	//Example of how to check if the player is in a specific zone
	//sf::FloatRect zoneBounds = zonesWorldBounds["Map_ZoneTuto"]["Zone_Tuto"];
	std::unordered_map<std::string, std::unordered_map<std::string, sf::FloatRect>> zonesWorldBounds = WorldManager->GetZonesWorldBounds();
	sf::Vector2f playerPos = m_charaCtrl->GetPos();
	CameraManager::SetZoneZoom(false);
	sf::FloatRect zoneBounds;

	if (WorldManager->GetCurrentMapName() == "Map_tutoriel.json")
	{
		zoneBounds = zonesWorldBounds["Map_tutoriel.json"]["Zone_CamFixeTuto"];
		if (zoneBounds.contains(playerPos))
		{
			m_camera->SetCameraZoneFixedTarget({ zoneBounds.left + zoneBounds.width / 2, zoneBounds.top + zoneBounds.height / 2 });
			m_camera->SetCameraZoneFixedView(true);
			CameraManager::SetZoneZoom(true);
			CameraManager::SetZoneZoomGoal(1.5f);
		}
		else
		{
			m_camera->SetCameraZoneFixedView(false);
		}

		if (!m_hookGot)
		{
			zoneBounds = zonesWorldBounds["Map_tutoriel.json"]["Tuto_Unlock_Hook"];
			if (zoneBounds.contains(playerPos))
			{
				m_charaCtrl->SetCanShootGrapple(true);

				DialogMaster::TrySetDialog(*m_hookDialog);
				delete m_hookDialog;
				m_hookDialog = DialogMaster::GetCurrentDialog();
				DialogMaster::SetEnabled(true);

				m_hookGot = true;
			}
		}
		else
		{
			zoneBounds = zonesWorldBounds["Map_tutoriel.json"]["Tuto_Clear_HookDialog"];
			if (zoneBounds.contains(playerPos))
			{
				if (DialogMaster::GetCurrentDialog() == m_hookDialog)
				{
					DialogMaster::SetEnabled(false);
				}
			}
		}

		if (!m_attackGot)
		{
			zoneBounds = zonesWorldBounds["Map_tutoriel.json"]["Tuto_Unlock_Attack"];
			if (zoneBounds.contains(playerPos))
			{
				m_charaCtrl->SetCanAttackSide(true);
				m_charaCtrl->SetCanAttackUp(true);
				m_charaCtrl->SetCanAttackDown(true);

				DialogMaster::TrySetDialog(*m_attackDialog);
				delete m_attackDialog;
				m_attackDialog = DialogMaster::GetCurrentDialog();
				DialogMaster::SetEnabled(true);

				m_attackGot = true;
			}
		}
		else
		{
			zoneBounds = zonesWorldBounds["Map_tutoriel.json"]["Tuto_Clear_AttackDialog"];
			if (zoneBounds.contains(playerPos))
			{
				if (DialogMaster::GetCurrentDialog() == m_attackDialog)
				{
					DialogMaster::SetEnabled(false);
				}
			}
		}

		if (!m_firstDialog)
		{
			zoneBounds = zonesWorldBounds["Map_tutoriel.json"]["Tuto_Initial_Dialog"];
			if (zoneBounds.contains(playerPos))
			{
				CutsceneMaster::AddInstruction(new CutsceneSetPlayerActions(false));
				Dialog* dialog = new Dialog();
				dialog->name = "Alex Harper";
				dialog->text = "Okay, Digger,\nYou are now in the cave.\nTell me when you are ready to explore\nby using your E button.";
				dialog->instant = false;
				dialog->speed = 20.f;
				CutsceneMaster::AddInstruction(new CutsceneDialog(dialog));
				CutsceneMaster::AddInstruction(new CutsceneSetPlayerActions(true));
				CutsceneMaster::AddInstruction(new CutsceneDialogDisable());
				m_firstDialog = true;
			}
		}
	}
	else if (WorldManager->GetCurrentMapName() == "Map_Chase.json")
	{
		zoneBounds = zonesWorldBounds["Map_Chase.json"]["Chase_Camera_ZoomOut"];
		if (zoneBounds.contains(playerPos))
		{
			m_camera->SetZoomGoal(1.5f);
			if (!BossChase::BossSpawned)
			{
				SoundManager::GetInstance()->Pause("FirstBiomeTheme");
				SoundManager::GetInstance()->Pause("AmbientCave");
				BossChase::BossSpawned = true;
				Entity* boss = GenerateEntity<BossChase>(m_bossSpawnLocation);
				m_entityManager->Add(boss);
				EntityState* state = new BossChaseRise(boss, 1200.f, 2.f);
				EntityState* chaseState = new BossChaseState(boss);

				sf::Sprite* sprite = boss->GetSprite();
				sf::Texture* texture = TextureManager::GetTexture("Boss_Spritesheet_Yell.png");

				Animation* yellAnimation = new Animation(CreateAnimation(*sprite, *texture, 24, 24.f, { 0,0,800,800 }));
				yellAnimation->loop = false;

				auto func = [this](void)-> void {m_displayHUD = false; };

				std::queue<CutsceneInstruction*> instructions;

				sf::Vector2f screenSize = SCREEN_SIZE * 1.5f;
				screenSize *= (1.f / 6.f);

				instructions.push(new CutsceneSetPlayerActions(false));
				instructions.push(new CutsceneSetValueLambda([]()->void
					{
						GameData::CharaCtrl->DestroyGrabhook();
						GameData::CharaCtrl->SetLinearVelocity({ 0,0 });
					}
				));
				instructions.push(new CutsceneCameraGoTo(BossChase::CinematicCameraBegin + sf::Vector2f{ screenSize.x, -screenSize.y }));
				//instructions.push(new CutscenePlayerTeleport(BossChase::PlayerCinematicSpawnBegin - sf::Vector2f{0, 16}));

				instructions.push(new CutsceneSetValueLambda(func));
				instructions.push(new CutsceneControlFrame(&m_cinematicFrames, true, 0.5f, m_cinematicFrameSize));

				instructions.push(new CutsceneEntitySetState(boss, state));
				instructions.push(new CutsceneWaitForStateEnd(boss));

				instructions.push(new CutsceneCameraShake(0.f, 180.f, 35.f, 1.5f));
				instructions.push(new CutscenePlaySound("BossScream"));
				instructions.push(new CutscenePlayAnimation(boss->GetAnimator()[0], yellAnimation, true));
				instructions.push(new CutsceneDelay(1.f));
				instructions.push(new CutsceneEntitySetState(boss, chaseState));
				instructions.push(new CutsceneDelay(0.2f));
				instructions.push(new CutsceneCameraReset());
				instructions.push(new CutsceneSetPlayerActions(true));
				instructions.push(new CutsceneSetValueLambda([this]()->void {m_displayHUD = true; }));
				instructions.push(new CutsceneControlFrame(&m_cinematicFrames, false, 0.5f, m_cinematicFrameSize));
				SoundManager::GetInstance()->Play("BossChase");
				CutsceneMaster::SetInstructions(instructions);
			}
		}

		zoneBounds = zonesWorldBounds["Map_Chase.json"]["Chase_Camera_ZoomIn1"];
		if (zoneBounds.contains(playerPos)) {
			{
				m_camera->SetZoomGoal(1.0f);
			}
		}

		zoneBounds = zonesWorldBounds["Map_Chase.json"]["Chase_Camera_ZoomIn2"];
		if (zoneBounds.contains(playerPos))
		{
			if (!BossChase::BossBeaten && BossChase::Boss != nullptr && !m_bossChaseDone)
			{
				m_bossChaseDone = true;

				std::queue<CutsceneInstruction*> instructions;
				SetValueDelegate* valueDelegate = new SetValueDelegate(std::bind(&CameraManager::SetZoomGoal, m_camera, std::placeholders::_1));

				sf::Vector2f screenSize = SCREEN_SIZE * 1.5f;
				screenSize *= (1.f / 6.f);

				sf::Sprite* sprite = BossChase::Boss->GetSprite();
				sf::Texture* texture = TextureManager::GetTexture("Boss_Spritesheet_Yell.png");

				Animation* yellAnimation = new Animation(CreateAnimation(*sprite, *texture, 24, 24.f, { 0,0,800,800 }));
				yellAnimation->loop = false;

				//SoundManager::GetInstance()->Stop("BossChase");

				instructions.push(new CutsceneSetPlayerActions(false));
				instructions.push(new CutsceneSetValueLambda([]()->void
					{
						GameData::CharaCtrl->DestroyGrabhook();
						GameData::CharaCtrl->SetLinearVelocity({ 0,0 });
					}
				));

				instructions.push(new CutsceneCameraGoTo(BossChase::CinematicCameraEnd + sf::Vector2f{ screenSize.x, 0 }));
				instructions.push(new CutsceneSetValueLambda([this]()->void {m_displayHUD = false; }));
				instructions.push(new CutsceneControlFrame(&m_cinematicFrames, true, 0.5f, m_cinematicFrameSize));

				//instructions.push(new CutscenePlayerTeleport(BossChase::PlayerCinematicSpawnEnd + sf::Vector2f{32, -16.f}));
				instructions.push(new CutsceneBooleanLambda([]()->bool {return BossChase::BossReachedEnd; }));
				instructions.push(new CutsceneCameraShake(0.f, 180.f, 35.f, 1.5f));
				instructions.push(new CutscenePlaySound("BossScream"));
				instructions.push(new CutscenePlayAnimation(BossChase::Boss->GetAnimator()[0], yellAnimation, true));
				instructions.push(new CutsceneDelay(1.f));
				instructions.push(new CutsceneEntitySetState(BossChase::Boss, new BossChaseEnd(BossChase::Boss)));
				instructions.push(new CutsceneDelay(0.2f));
				instructions.push(new CutsceneWaitForStateEnd(BossChase::Boss));

				instructions.push(new CutsceneSetValueLambda([]()->void {SoundManager::GetInstance()->Stop("BossChase"); }));
				instructions.push(new CutscenePlaySound("FirstBiomeTheme"));
				instructions.push(new CutscenePlaySound("AmbientCave"));

				instructions.push(new CutsceneCameraReset());
				instructions.push(new CutsceneSetValueDelegate(1.f, valueDelegate, true));
				instructions.push(new CutsceneSetValueLambda([this]()->void {m_displayHUD = true; }));
				instructions.push(new CutsceneControlFrame(&m_cinematicFrames, false, 0.5f, m_cinematicFrameSize));
				instructions.push(new CutsceneSetPlayerActions(true));
				instructions.push(new CutsceneDelay(0.5f));




				CutsceneMaster::SetInstructions(instructions);
			}
			else if (CutsceneMaster::InstructionsDone())
			{
				m_camera->SetZoomGoal(1.f);
			}
		}
	}
	else if (WorldManager->GetCurrentMapName() == "Map_Extra.json")
	{
		zoneBounds = zonesWorldBounds["Map_Extra.json"]["ZoneContemplatif"];
		if (zoneBounds.contains(playerPos))
		{
			m_camera->SetZoneZoomGoal(2.0f);
			m_camera->SetZoneZoom(true);
		}
		else
		{
			m_camera->SetZoneZoom(false);
		}
	}
	else if (WorldManager->GetCurrentMapName() == "Map_Boss_Arena.json")
	{
		SoundManager::GetInstance()->Play("AmbientBoss", true);
		SoundManager::GetInstance()->Stop("FirstBiomeTheme");
		SoundManager::GetInstance()->Stop("AmbientCave");
		zoneBounds = zonesWorldBounds["Map_Boss_Arena.json"]["Boss_ArenaBox"];
		if (!Boss::BossBeaten && zoneBounds.contains(playerPos))
		{
			Boss::BossArena = zoneBounds;
			m_camera->SetZoneZoomGoal(2.f);
			m_camera->SetZoneZoom(true);
			m_camera->SetCameraZoneFixedTarget({ zoneBounds.left + zoneBounds.width / 2, zoneBounds.top + zoneBounds.height / 2 });
			m_camera->SetCameraZoneFixedView(true);
		}
		else
		{
			m_camera->SetZoneZoom(false);
			m_camera->SetCameraZoneFixedView(false);
		}

		zoneBounds = zonesWorldBounds["Map_Boss_Arena.json"]["Boss_SpawnArea"];
		if (!Boss::BossSpawned && zoneBounds.contains(playerPos))
		{
			std::queue<CutsceneInstruction*> instructions;
			Boss::BossSpawned = true;
			m_boss = GenerateEntity<Boss>(Boss::BossSpawnPoint);
			m_entityManager->Add(m_boss);
			m_bossLifeBar = new sf::RectangleShape();
			m_bossLifeBar->setSize({ SCREEN_SIZE.x * 0.6f, 35.f });
			m_bossLifeBar->setFillColor(sf::Color::Red);
			m_bossLifeBar->setPosition({ SCREEN_SIZE.x * 0.2f, SCREEN_SIZE.y * 0.9f });
			m_bossMaxHealth = m_boss->GetMaxHealth();

			instructions.push(new CutsceneSetPlayerActions(false));
			instructions.push(new CutsceneCameraShake(0.f, 2.f, 1.f, 1.5f));
			instructions.push(new CutscenePlaySound("BossScream"));
			instructions.push(new CutsceneDelay(1.5f));
			instructions.push(new CutsceneSetPlayerActions(true));
			instructions.push(new CutscenePlaySound("BossTheme"));

			CutsceneMaster::SetInstructions(instructions);
			SoundManager::GetInstance()->Stop("AmbientBoss");
		}

		zoneBounds = zonesWorldBounds["Map_Boss_Arena.json"]["Boss_SpawnArea2"];
		if (!Boss::BossSpawned && zoneBounds.contains(playerPos))
		{
			std::queue<CutsceneInstruction*> instructions;
			Boss::BossSpawned = true;
			m_boss = GenerateEntity<Boss>(Boss::BossSpawnPoint);
			m_entityManager->Add(m_boss);
			m_bossLifeBar = new sf::RectangleShape();
			m_bossLifeBar->setSize({ SCREEN_SIZE.x * 0.6f, 35.f });
			m_bossLifeBar->setFillColor(sf::Color::Red);
			m_bossLifeBar->setPosition({ SCREEN_SIZE.x * 0.2f, SCREEN_SIZE.y * 0.9f });
			m_bossMaxHealth = m_boss->GetMaxHealth();

			instructions.push(new CutsceneSetPlayerActions(false));
			instructions.push(new CutsceneCameraShake(0.f, 2.f, 1.f, 1.5f));
			instructions.push(new CutscenePlaySound("BossScream"));
			instructions.push(new CutsceneDelay(1.5f));
			instructions.push(new CutsceneSetPlayerActions(true));
			instructions.push(new CutscenePlaySound("BossTheme"));

			CutsceneMaster::SetInstructions(instructions);
			SoundManager::GetInstance()->Stop("AmbientBoss");

		}
	}
}

void MineState::UpdateCurrentMapPlayer()
{
	sf::Vector2f playerPos = m_charaCtrl->GetPos();
	std::map<std::string, tson::WorldMapData> worldDataMap = WorldManager->GetWorldDataMap();

	std::string currentMapName = WorldManager->GetCurrentMapName();
	std::string previousMapName = WorldManager->GetPreviousMapName();


	for (auto& [name, worldData] : worldDataMap)
	{
		bool isPlayerOnMap = worldData.position.x <= playerPos.x && playerPos.x <= worldData.position.x + worldData.size.x &&
			worldData.position.y <= playerPos.y && playerPos.y <= worldData.position.y + worldData.size.y;

		if (isPlayerOnMap)
		{
			if ((currentMapName != name))
			{
				m_camera->SetLeftLimit((float)worldData.position.x);
				m_camera->SetRightLimit((float)(worldData.position.x + worldData.size.x));
				m_camera->SetUpLimit((float)worldData.position.y);
				m_camera->SetBottomLimit((float)(worldData.position.y + worldData.size.y));

				if (!(m_isPlayerSpawned))
				{
					PushState(new FadeInState(0.5f));
					m_isPlayerSpawned = true;
				}
				WorldManager->SetCurrentMapName(name);
				WorldManager->SetCurrentMapData(name);

				//Reset the previous map name 
				//WorldManager->SetPreviousMapName("");
			}


			break;
		}
	}

	if (currentMapName != WorldManager->GetCurrentMapName())
	{
		WorldManager->SetPreviousMapName(currentMapName);
	}
}

void MineState::InitDialogs()
{

	m_hookDialog = new Dialog();
	m_hookDialog->name = "Alex Harper";
	m_hookDialog->text = "I've just given you access to the hook. \nTry it by using the right click.";
	m_hookDialog->instant = false;
	m_hookDialog->speed = 20.f;

	m_attackDialog = new Dialog();
	m_attackDialog->name = "Alex Harper";
	m_attackDialog->text = "Your drill is now functional \nTry using your left click.";
	m_attackDialog->instant = false;
	m_attackDialog->speed = 20.f;

	DialogMaster::InitialiseDialogMaster();
	DialogMaster::SetEnabled(false);
}

void MineState::InitGUI()
{
	GUIImage::TextureSearchFunction = &TextureManager::GetTexture;

	m_lifePoints = new sf::Text;
	sf::Font* font = new sf::Font;

	font->loadFromFile("Assets/Fonts/Orbitron-ExtraBold.ttf");
	GUIText::Font = font;
	//GUIText::Font->loadFromFile("Assets/Fonts/Orbitron-ExtraBold.ttf");

	std::ifstream dialogsFile("ConfigFiles/UI/MineUI.dat");
	if (dialogsFile.is_open())
	{
		m_guiManager->Load(dialogsFile);
		dialogsFile.close();
	}

	// Création de l'objet sf::Text
	m_lifePoints->setFont(*font); // Définition de la police
	m_lifePoints->setString(std::to_string(m_charaCtrl->GetStats()->GetHp())); // Définition du texte
	m_lifePoints->setCharacterSize(24); // Définition de la taille du texte
	m_lifePoints->setFillColor(sf::Color::White); // Définition de la couleur du texte
	m_lifePoints->setPosition(lifePointOriginalPos);
	GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");
	pauseGUI->SetEnabled(false);

	((GUIText*)m_guiManager->GetNodeByTag("DialogText"))->SetFontSize(35);

	m_cinematicFrames = new sf::RectangleShape[2];
	m_cinematicFrames[0].setSize({ SCREEN_SIZE.x, m_cinematicFrameSize });
	m_cinematicFrames[0].setFillColor(sf::Color(0, 0, 0, 255));
	m_cinematicFrames[0].setPosition({ 0, -m_cinematicFrameSize });

	m_cinematicFrames[1].setSize({ SCREEN_SIZE.x, m_cinematicFrameSize });
	m_cinematicFrames[1].setFillColor(sf::Color(0, 0, 0, 255));
	m_cinematicFrames[1].setPosition({ 0, SCREEN_SIZE.y });

	m_interactButtonBackground = new sf::Sprite;
	m_interactButtonBackground->setTexture(*TextureManager::GetTexture("HUD_Inventory_box_02.png"));
	m_interactButtonBackground->setOrigin(37, 37);

	m_interactButtonText = new sf::Text;
	m_interactButtonText->setString("E");
	m_interactButtonText->setCharacterSize(45);
	m_interactButtonText->setOrigin((float)m_interactButtonText->getCharacterSize() / 2.f - 5.f, (float)(m_interactButtonText->getCharacterSize() / 2.f) + 7.f);
	m_interactButtonText->setFillColor(sf::Color::White);
	m_interactButtonText->setFont(*GUIText::Font);

	GUIText* tempText = (GUIText*)m_guiManager->GetNodeByTag("PlayText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("PlayButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("SaveText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("SaveButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("textSettings");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("settingsButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("BackMenuText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("BackMenuButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("LoadText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("LoadButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("fullText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("Fullscre")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	tempText = (GUIText*)m_guiManager->GetNodeByTag("BackText");
	tempText->SetInMiddle(*m_guiManager->GetNodeByTag("BackButton")->GetSfShape());
	tempText->SetSizeUp(true);
	tempText->SetFontSizeUpSize(tempText->GetFontSize() + 7);

	m_volumeSlider = new Slider((int)SCREEN_SIZE.x / 2 - 250, (int)SCREEN_SIZE.y / 2 + 100);
	m_volumeSlider->create(0, 100);
	m_volumeSlider->setSliderValue(SoundManager::GetInstance()->GetVolume());

	if (GameData::fullscreen)
	{
		GUIManager::GetInstance()->GetNodeByTag("togglePoint")->SetEnabled(true);
	}
	else
	{
		GUIManager::GetInstance()->GetNodeByTag("togglePoint")->SetEnabled(false);
	}
}

void MineState::InitSound()
{
	SoundManager::GetInstance()->StopAll();
	SoundManager::GetInstance()->ChangeState(GAME);
	SoundManager::GetInstance()->Play("AmbientCave");
	SoundManager::GetInstance()->Play("FirstBiomeTheme");
	SoundManager::GetInstance()->Play("WindSpeed");
	SoundManager::GetInstance()->SetVolumeAccordingToValue("WindSpeed", 0);
}

void MineState::InitCharaCtrl()
{
	m_charaCtrl = new CharaControl(GameData::CharacterStats);
	GameData::CharaCtrl = m_charaCtrl;
	m_charaCtrl->SetPos(WorldManager->GetPlayerWorldPos() - sf::Vector2f{ 0, 50.f });

	if (m_hookGot)
	{
		m_charaCtrl->SetCanShootGrapple(true);
	}

	if (m_attackGot)
	{
		m_charaCtrl->SetCanAttackSide(true);
		m_charaCtrl->SetCanAttackUp(true);
		m_charaCtrl->SetCanAttackDown(true);
	}

	m_charaCtrl->GetStats()->SetHp(50);
	m_charaCtrl->GetStats()->LoadStatsJSON("ConfigFiles/Character/CharacterData.json");
	m_charaCtrl->GetStats()->LoadBoostJSON("ConfigFiles/Character/CharacterBoostData.json");
}

void MineState::InitManagers()
{
	TextureManager::LoadTextures("Assets/Sprites");
	ItemManager::Init("ConfigFiles/Data/ItemData.json");
	WorldManager->Init();
	MiniMapManager::GetInstance()->Init();
	m_entityManager = EntityManager::GetInstance();
	m_guiManager = GUIManager::GetInstance();


}

void MineState::InitCamera()
{
	std::unordered_map<std::string, std::unordered_map<std::string, sf::FloatRect>> zonesWorldBounds = WorldManager->GetZonesWorldBounds();
	sf::FloatRect zoneBounds;;
	zoneBounds = zonesWorldBounds["Map_tutoriel.json"]["Camera_Bounds"];
	m_camera = CameraManager::GetInstance();
	m_camera->SetCharacter(m_charaCtrl);
	m_camera->SetZoomGoal(1.f);
	m_camera->SetZoom(1.f);

	m_camera->SetLeftLimit(zoneBounds.left);
	m_camera->SetRightLimit(zoneBounds.left + zoneBounds.width);
	m_camera->SetUpLimit(zoneBounds.top);
	m_camera->SetBottomLimit(zoneBounds.top + zoneBounds.height);
	m_camera->Update();
}

