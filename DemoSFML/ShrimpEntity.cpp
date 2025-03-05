#include "ShrimpEntity.h"

#include "ShrimpIdle.h"
#include "ShrimpHurt.h"
#include "ShrimpDeath.h"

#include "Animator.h"
#include "TextureManager.h"
#include "Animation.h"

#include "Box2D_Collision.h"
#include "Box2D_Shape.h"

#include "common.h"
#include "CharacterController.h"
#include "CharacterStats.h"
#include "CameraManager.h"
#include "EntityManager.h"
#include "ItemManager.h"
#include "SoundManager.h"
#include "ParticleManager.h"
#include "StandardParticles.h"
#include "TimeSpeedManager.h"

ShrimpEntity::ShrimpEntity(sf::Vector2f _pos)
{
	b2Filter filter;
	GetPositionFunc* posFunc = nullptr;
	b2::Shape* shape = nullptr;
	b2::CollisionCallback* callback = nullptr;

	filter.categoryBits = F_MONSTER;
	filter.maskBits = 0xFFFF - F_MONSTER;

	m_weight = 10.f;
	m_sightRange = 30.f;
	m_cowardly = false;
	m_flying = true;
	m_maxHealth = 1;
	m_health = m_maxHealth;
	m_position = _pos;
	m_invulnTime = 0.f;

	m_loots = new LootDef();

	m_bodyCount = 1;
	m_bodies = new b2Body *[1];
	m_shapes = new b2::Shape *[1];
	m_offsets = new sf::Vector2f[1];

	callback = new b2::CollisionCallback;
	*callback = std::bind(&ShrimpEntity::HurtboxCollided, this, std::placeholders::_1);

	shape = new b2::Circle(*GameData::World, b2_dynamicBody, m_position, 50, {0,0}, "Hurtbox", (uintptr_t)(this), callback);
	m_shapes[0] = shape;

	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { 0,0 };
	shape->SetPhysics(10.f, 0.f, false, 0.f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);

	m_sprite = new sf::Sprite();
	m_sprite->setTextureRect({ 0, 0, 150, 150 });
	m_sprite->setOrigin(75, 75);

	posFunc = new GetPositionFunc();
	*posFunc = std::bind(&ShrimpEntity::GetScreenPos, this);
	m_animators[0] = new Animator(posFunc);

	LootStats stats;
	stats.chance = 1.f;
	stats.min = 1;
	stats.max = 3;
	m_loots->onDeath[ItemManager::GetItem("Rock")] = stats;

	GenerateBaseAnimations(false);

	m_state = new ShrimpIdle(this);
}

ShrimpEntity::ShrimpEntity(const EntityDef& _def, sf::Vector2f _pos) : Entity(_def, _pos)
{
	m_state = new ShrimpIdle(this);
}

void ShrimpEntity::HurtboxCollided(b2::UserData* _data)
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
			ChangeState(new ShrimpHurt(this, character->GetStats()->GetDamage()));
		}
		else
		{
			ChangeState(new ShrimpDeath(this));
			SoundManager::GetInstance()->Play("ShrimpDeath");

			TimeSpeedManager::SetBreakSpeed(0.f);
			TimeSpeedManager::SetBreakTime(0.35f, 0.65f);

			CameraManager::PrepareForShake(angleRad, 0.1f, 25.f);
			CameraManager::Shake(0.3f);
		}

		m_invulnTime = 0.5f;

		sf::Sprite* sprite = new sf::Sprite();
		sprite->setOrigin(425, 425);
		sprite->setTextureRect({ 0, 0, 850, 850 });
		sprite->setRotation(angleRad * 180.f / 3.1415f);
		sprite->setScale(0.5f, 0.5f);
		sf::Texture* texture = TextureManager::GetTexture("AttackImpactFX.png");
		Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 6, 15.f, { 0,0,850,850 }));
		ParticleManager::AddParticle(new StandardParticle(0.f, 0.f, 1.f, playerPos + (path * 0.8f), animation));
	}
}

void ShrimpEntity::GenerateBaseAnimations(bool _background)
{
	Animator& animator = *m_animators[0];
	Animation* animation = nullptr;
	sf::Texture* texture = nullptr;

	texture = TextureManager::GetTexture("Shrimp_Spritesheet.png");
	animation = new Animation(CreateAnimation(*m_sprite, *texture, 24, 24.f, { 0,0,150,150 }));
	animator.AddAnimation("Idle", animation);

	animation = new Animation(CreateAnimation(*m_sprite, *texture, 1, 1.f, { 1050,0,150,150 }));
	animation->loop = false;
	animator.AddAnimation("Hurt", animation);

	texture = TextureManager::GetTexture("Shrimp_Death_Spritesheet.png");
	animation = new Animation(CreateAnimation(*m_sprite, *texture, 13, 24.f, { 0,0,150,150 }));
	animation->loop = false;
	animator.AddAnimation("Death", animation);
}
