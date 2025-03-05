#include "BossChase.h"
#include "Box2D_Collision.h"
#include "Box2D_Shape.h"
#include "common.h"
#include "BossChaseStateInclude.h"
#include "GameState.h"
#include "FadeinState.h"
#include "EntityManager.h"
#include "CharacterController.h"
#include "CameraManager.h"
#include "TextureManager.h"
#include "Animation.h"
#include "Animator.h"
#include "SoundManager.h"

Entity* BossChase::Boss = nullptr;
sf::Vector2f BossChase::RespawnPoint = { 0,0 };
sf::Vector2f BossChase::EndPoint = { 0,0 };
bool BossChase::BossSpawned = false;
bool BossChase::BossReachedEnd = false;
bool BossChase::BossBeaten = false;

sf::Vector2f BossChase::PlayerCinematicSpawnBegin = { 0,0 };
sf::Vector2f BossChase::PlayerCinematicSpawnEnd = { 0,0 };
sf::Vector2f BossChase::CinematicCameraBegin = { 0,0 };
sf::Vector2f BossChase::CinematicCameraEnd = { 0,0 };

BossChase::BossChase(sf::Vector2f _pos)
{
	b2Filter filter;
	b2::Shape* shape;
	b2::CollisionCallback* callback;
	GetPositionFunc* posFunc;

	filter.categoryBits = F_MONSTER;
	filter.maskBits = F_PLAYER;

	m_weight = 100.f;
	m_sightRange = 1000.f;
	m_cowardly = false;
	m_flying = true;
	m_maxHealth = 1000;
	m_health = m_maxHealth;
	m_position = _pos;
	m_invulnTime = 10.f;
	m_loots = new LootDef();

	m_bodyCount = 1;
	m_bodies = new b2Body*[m_bodyCount];
	m_shapes = new b2::Shape*[m_bodyCount];
	m_offsets = new sf::Vector2f[m_bodyCount];

	callback = new b2::CollisionCallback();
	*callback = std::bind(&BossChase::HitboxCollided, this, std::placeholders::_1);

	shape = new b2::Rectangle(*GameData::World, b2_kinematicBody, m_position, { 616,552 }, "BossHitbox", (uintptr_t)(this), callback);
	m_shapes[0] = shape;
	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { 0,0 };
	shape->SetPhysics(1.0f, 0.f, true, 0.f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);

	m_state = new BossChaseWait(this, -1.f, nullptr);
	Boss = this;

	posFunc = new GetPositionFunc();
	*posFunc = std::bind(&BossChase::GetScreenPos, this);
	m_animators.push_back(new Animator(posFunc));
}

BossChase::~BossChase()
{
	if (Boss == this)
	{
		Boss = nullptr;
	}
}

void BossChase::HitboxCollided(b2::UserData* _data)
{
	if (_data->name == "Chara")
	{
		BossSpawned = false;
		BossReachedEnd = false;
		GameState::PushState(new FadeInState());
		m_respawnPlayer = true;
		GameData::CharaCtrl->AddDamage(10);
		EntityManager::GetInstance()->MarkForDeath(this);
		SoundManager::GetInstance()->Stop("BossChase");
	}
}

void BossChase::GenerateBaseAnimations(bool _background)
{
	sf::Vector2f frameSize = { 800, 800 };
	Animation* anim;
	sf::Sprite* sprite = m_sprite;
	sf::Texture* texture = TextureManager::GetTexture("Boss_Spritesheet_Idle.png");

	anim = new Animation(CreateAnimation(*sprite, *texture, 24, 24.f, { 0,0,(int) frameSize.x, (int) frameSize.y }));
	m_animators[0]->AddAnimation("Idle", anim);

	texture = TextureManager::GetTexture("Boss_Spritesheet_Move.png");
	anim = new Animation(CreateAnimation(*sprite, *texture, 24, 24.f, { 0,0,(int)frameSize.x, (int)frameSize.y }));
	m_animators[0]->AddAnimation("Move", anim);
	
	texture = TextureManager::GetTexture("Boss_Spritesheet_Dash.png");
	anim = new Animation(CreateAnimation(*sprite, *texture, 24, 24.f, { 0,0,(int)frameSize.x, (int)frameSize.y }));
	m_animators[0]->AddAnimation("Dash", anim);

	m_animators[0]->TrySetCurrentAnimation("Idle");

	m_sprite->setOrigin({ frameSize.x / 2.f, frameSize.y / 2.f });
}

void BossChase::Update()
{
	Entity::Update();

	if (m_respawnPlayer)
	{
		GameData::CharaCtrl->SetPos(RespawnPoint);
		CameraManager::GetInstance()->SetCharacter(GameData::CharaCtrl);
	}
}
