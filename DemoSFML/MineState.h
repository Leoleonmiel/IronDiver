#ifndef MINE_STATE__H
#define MINE_STATE__H

//#include <SFML/Graphics.hpp>
#include <list>
#include "GameState.h"

class CharaControl;
class Collectable;	
class CameraManager;
class CharacterStats;
class Animator;
struct Animation;
class TiledManager;
class Entity;
class PlayerInventory;
namespace b2 { class Rectangle; }
class EntityManager;
class GUIManager;
class Dialog;
class ParticleEnvironment;
class ParticleSystem;
class ParticleEmitter;
class Slider;
class ShadersManager;

class MineState : public GameState
{
public:
	MineState();
	void Init() override;
	void Update(float) override;
	void Quit() override;
	void Display(sf::RenderWindow&) override;
	void HandleEvents(sf::Event&) override;

	static bool NearTeleporter;
	static sf::Vector2f CloseTeleporterPos;
	static ParticleSystem* ExplosionSystem;
	static ParticleEmitter* ExplosionEmitter;
	static sf::RectangleShape* m_cinematicFrames;
	ShadersManager* m_shadersManager;
private:
	void UpdateTeleporters();
	void InitEntities();
	void UpdateCameraWorldZoom();
	void UpdateCurrentMapPlayer();
	
	//Used in Init()
	void InitDialogs();
	void InitGUI();

	//Used in Constructor
	void InitSound();
	void InitCharaCtrl();
	void InitManagers();
	void InitCamera();

	ParticleEnvironment* m_particleEnvironment = nullptr;
	

	CharaControl* m_charaCtrl = nullptr;
	CameraManager* m_camera = nullptr;

	Animator* m_tpAnimator = nullptr;
	Animation* m_tpAnim = nullptr;

	std::list<Collectable*> m_collectables;

	sf::Text* m_healthAmount = nullptr;

	Slider* m_volumeSlider = nullptr;

	//CharacterStats* m_characterStats = nullptr;
	EntityManager* m_entityManager = nullptr;

	sf::RectangleShape* lifeBar = nullptr;
	sf::Vector2f fullLifeSize = sf::Vector2f(330, 32);

	sf::Vector2f m_bossSpawnLocation = sf::Vector2f(0, 0);

	GUIManager* m_guiManager = nullptr;
	float m_timer = 0.f;

	sf::Sprite* m_backgroundTemp = nullptr;
	sf::Sprite* m_tpAnimSprite = nullptr;

	Dialog* m_hookDialog = nullptr;
	Dialog* m_attackDialog = nullptr;
	sf::RectangleShape* m_bossLifeBar = nullptr;
	Entity* m_boss = nullptr;
	
	sf::Sprite* m_hudHPSprite = nullptr;
	sf::Sprite* m_hudRedHPSprite = nullptr;
	float m_cinematicFrameSize = 100;
	int m_bossMaxHealth = 0;

	// Story Flags
	static bool m_firstDialog;
	static bool m_hookGot;
	static bool m_attackGot;

	bool m_isPlayerSpawned = false;
	bool m_isGamePaused = false;
	bool m_teleporting = false;
	bool m_bossChaseDone = false;
	bool m_displayHUD = true;
	int m_currentTeleporter = 0;
	float m_lastGameSpeed = 0;
	
	sf::Sprite* m_interactButtonBackground = nullptr;
	sf::Text* m_interactButtonText = nullptr;

	float m_elapsedTimeLifeBar = 0.f;
	sf::Color m_startLifeBarColor = sf::Color(48, 136, 197);
	/*sf::Vector2f lifeBarHUDOriginalPos = sf::Vector2f(270, 80);
	sf::Vector2f lifeBarOriginalPos = sf::Vector2f(102, 63);*/

	sf::Sprite* m_lifeBarSprites[11];
	sf::Sprite* m_lifeBarRedSprites[6];
	sf::Text* m_lifePoints = nullptr;

	sf::Vector2f lifeBarHUDOriginalPos = sf::Vector2f(180, 920);
	sf::Vector2f lifePointOriginalPos = sf::Vector2f(132, 905);

};
#endif
