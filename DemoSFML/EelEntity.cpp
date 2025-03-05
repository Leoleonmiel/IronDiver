#include "EelEntity.h"
#include <queue>
#include "Animation.h"
#include "Animator.h"
#include "TextureManager.h"
#include "EelStateInclude.h"
#include "CharacterController.h"
#include "CameraManager.h"
#include "CharacterStats.h"
#include "SoundManager.h"
#include "Box2D_Collision.h"
#include "Box2D_Shape.h"
#include "TimeSpeedManager.h"
#include "ParticleManager.h"
#include "StandardParticles.h"
#include "ItemManager.h"

EelEntity::~EelEntity()
{
}

EelEntity::EelEntity(sf::Vector2f _pos)
{
	b2Filter filter;
	GetPositionFunc* posFunc = nullptr;
	b2::Shape* shape = nullptr;
	b2::CollisionCallback* callback = nullptr;

	filter.categoryBits = F_MONSTER;
	filter.maskBits = 0xFFFF - F_MONSTER;

	m_weight = 10.f;
	m_sightRange = 35.f;
	m_cowardly = false;
	m_flying = true;
	m_maxHealth = 2;
	m_health = m_maxHealth;
	m_position = _pos;
	m_invulnTime = 0.f;

	m_loots = new LootDef();

	m_bodyCount = 2;
	m_bodies = new b2Body * [m_bodyCount];
	m_shapes = new b2::Shape * [m_bodyCount];
	m_offsets = new sf::Vector2f[m_bodyCount];

	callback = new b2::CollisionCallback;
	*callback = std::bind(&EelEntity::HurtboxCollided, this, std::placeholders::_1);

	shape = new b2::Rectangle(*GameData::World, b2_dynamicBody, m_position, { 240,50 }, "Eel_Hurtbox", (uintptr_t)(this), callback);
	m_shapes[0] = shape;

	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { 0,0 };
	shape->SetPhysics(10.f, 0.f, false, 0.f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);

	callback = new b2::CollisionCallback;
	*callback = std::bind(&EelEntity::HitboxCollided, this, std::placeholders::_1);

	shape = new b2::Rectangle(*GameData::World, b2_kinematicBody, m_position, { 250,60 }, "Hitbox", (uintptr_t)(this), callback);
	m_shapes[1] = shape;

	m_bodies[1] = shape->GetBody();
	m_offsets[1] = { 0,0 };
	shape->SetPhysics(0.f, 100.f,true, 0.f);
	m_bodies[1]->SetGravityScale(0.f);
	m_bodies[1]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[1]->SetFixedRotation(true);
	m_bodies[1]->SetEnabled(false);

	m_sprite = new sf::Sprite();
	m_sprite->setTextureRect({ 0, 0, 250, 250 });
	m_sprite->setOrigin(125, 125);

	posFunc = new GetPositionFunc();
	*posFunc = std::bind(&EelEntity::GetScreenPos, this);
	m_animators[0] = new Animator(posFunc);
	m_animators.push_back(new Animator(posFunc));

	m_direction = float(rand() % 2 ? 1 : -1);

	LootStats stats;
	stats.chance = 1.f;
	stats.min = 1;
	stats.max = 3;
	m_loots->onDeath[ItemManager::GetItem("Etherium")] = stats;

	GenerateBaseAnimations(false);
	basePos = GetScreenPos();

	m_state = new EelIdle(this);
}

EelEntity::EelEntity(const EntityDef& _def, sf::Vector2f _pos)
{
	m_state = new EelIdle(this);
}

void EelEntity::HurtboxCollided(b2::UserData* _data)
{
	if (_data->name == "Attack" && m_health > 0 && m_invulnTime <= 0.f)
	{
		CharaControl* character = (CharaControl*)_data->ptr;
		sf::Vector2f playerPos = character->GetPos();
		sf::Vector2f entityPos = GetScreenPos();
		sf::Vector2f path = entityPos - playerPos;
		float angleRad = atan2f(path.y, path.x);

		TakeDamage(character->GetStats()->GetDamage());
		if (m_health > 0)
		{
			ChangeState(new EelHurt(this, character->GetStats()->GetDamage()));
			//SoundManager::GetInstance()->Play("FlyHurt");
		}
		else
		{
			ChangeState(new EelDeath(this));
			//SoundManager::GetInstance()->Play("FlyDeath");

			
			

			TimeSpeedManager::SetBreakSpeed(0.f);
			TimeSpeedManager::SetBreakTime(0.35f, 0.65f);
			

			CameraManager::PrepareForShake(angleRad, 0.1f, 25.f);
			CameraManager::Shake(0.3f);
		}

		sf::Sprite* sprite = new sf::Sprite();
		sprite->setOrigin(425, 425);
		sprite->setTextureRect({ 0, 0, 850, 850 });
		sprite->setRotation(angleRad * 180.f / 3.1415f);
		sprite->setScale(0.5f, 0.5f);
		sf::Texture* texture = TextureManager::GetTexture("AttackImpactFX.png");
		Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 6, 15.f, { 0,0,850,850 }));
		ParticleManager::AddParticle(new StandardParticle(0.f, 0.f, 1.f, playerPos + (path * 0.8f), animation));

		m_invulnTime = 0.5f;
	}
}

void EelEntity::GenerateBaseAnimations(bool _background)
{
	Animation* animation;
	std::queue<Animation*> animations;
	sf::Sprite* sprite = m_sprite;
	sf::Texture texture;

	GetPositionFunc* func = new GetPositionFunc();
	*func = std::bind(&EelEntity::GetScreenPos, this);

	texture = *TextureManager::GetTexture("Eel_Spritesheet.png");
	animation = new Animation(CreateAnimation(*sprite, texture, 7, 6.f, { 0,0,250,250 }));
	m_animators[0]->AddAnimation("PrepareInpulsion", animation);

	animations.push(animation);

	animation = new Animation(CreateAnimation(*sprite, texture, 3, 10.f, { 0,250,250,250 }));
	m_animators[0]->AddAnimation("Rush", animation);

	animation = new Animation(CreateAnimation(*sprite, texture, 6, 6.f, { 0,500,250,250 }));
	m_animators[0]->AddAnimation("Attack", animation);

	animation = new Animation(CreateAnimation(*sprite, texture, 3, 7.f, { 750,0,250,250 }));
	animation->loop = false;
	m_animators[0]->AddAnimation("Hurt", animation);

	animation = new Animation(CreateAnimation(*sprite, texture, 4, 15.f, { 0,1000,250,250 }));
	animation->loop = false;
	m_animators[0]->AddAnimation("WaitingDeath", animation);

	animation = new Animation(CreateAnimation(*sprite, texture, 9, 5.f, { 1000,1000,250,250 }));
	animation->loop = false;
	m_animators[0]->AddAnimation("Death", animation);


	m_animators[0]->SetAnimationQueue(animations);
}

void EelEntity::GetLookDirection() 
{
	m_lookingRight = true;
}

void EelEntity::Display()
{
	m_sprite->setScale(-m_scale.x, m_sprite->getScale().y);
	m_sprite->setRotation(m_rotation + m_bodies[0]->GetAngle() * 180.f / -3.14159f);
	for (Animator* animator : m_animators)
	{
		animator->Display(*GameData::Window);
	}
}

void EelEntity::Update()
{
	EntityState* state = nullptr;
	m_state->Update();
	state = m_state->GetNextState();
	if (state != nullptr)
	{
		ChangeState(state);
	}
	for (size_t i = 0; i < m_bodyCount; i++)
	{
		m_bodies[i]->SetTransform(Screen2World(GetScreenPos() + m_offsets[i]), m_angle);
	}
	if (m_invulnTime > 0.f)
	{
		m_invulnTime -= GameData::DeltaTime * GameData::GameSpeed;
	}

	GetLookDirection();
}
