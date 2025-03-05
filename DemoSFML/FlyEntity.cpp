#include "FlyEntity.h"
#include <queue>
#include "Animation.h"
#include "Animator.h"
#include "TextureManager.h"
#include "FlyStateInclude.h"
#include "CharacterController.h"
#include "CameraManager.h"
#include "CharacterStats.h"
#include "ItemManager.h"
#include "SoundManager.h"

#include "Box2D_Collision.h"
#include "Box2D_Shape.h"
#include "TimeSpeedManager.h"
#include "ParticleManager.h"
#include "StandardParticles.h"

FlyEntity::~FlyEntity()
{
}

FlyEntity::FlyEntity(sf::Vector2f _pos)
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
	*callback = std::bind(&FlyEntity::HurtboxCollided, this, std::placeholders::_1);

	shape = new b2::Rectangle(*GameData::World, b2_dynamicBody, m_position, { 90,90 }, "Hurtbox", (uintptr_t)(this), callback);
	m_shapes[0] = shape;

	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { 0,0 };
	shape->SetPhysics(10.f, 0.f, false, 0.f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);


	callback = new b2::CollisionCallback;
	*callback = std::bind(&FlyEntity::HitboxCollided, this, std::placeholders::_1);

	shape = new b2::Rectangle(*GameData::World, b2_kinematicBody, m_position, { 120,120 }, "Hitbox", (uintptr_t)(this), callback);
	m_shapes[1] = shape;

	m_bodies[1] = shape->GetBody();
	m_offsets[1] = { 0,0 };
	shape->SetPhysics(0.f, 100.f, true, 0.f);
	m_bodies[1]->SetGravityScale(0.f);
	m_bodies[1]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[1]->SetFixedRotation(true);
	m_bodies[1]->SetEnabled(false);

	m_sprite = new sf::Sprite();
	m_sprite->setTextureRect({ 0, 0, 300, 300 });
	m_sprite->setOrigin(150, 150);

	posFunc = new GetPositionFunc();
	*posFunc = std::bind(&FlyEntity::GetScreenPos, this);
	m_animators[0] = new Animator(posFunc);
	m_animators.push_back(new Animator(posFunc));

	LootStats stats;
	stats.chance = 1.f;
	stats.min = 1;
	stats.max = 3;
	m_loots->onDeath[ItemManager::GetItem("Rock")] = stats;

	GenerateBaseAnimations(false);

	m_state = new FlyIdle(this);
}

FlyEntity::FlyEntity(const EntityDef& _def, sf::Vector2f _pos)
{
	m_state = new FlyIdle(this);
}

void FlyEntity::HurtboxCollided(b2::UserData* _data)
{
	if (_data->name == "Attack" && m_health > 0 && m_invulnTime <= 0.f)
	{
		CharaControl* character = (CharaControl*)_data->ptr;
		sf::Vector2f playerPos = character->GetPos();
		sf::Vector2f entityPos = GetScreenPos();
		sf::Vector2f path = entityPos - playerPos;
		float angleRad = atan2f(path.y, path.x);

		ChangeState(new FlyHurt(this, character->GetStats()->GetDamage()));


		TakeDamage(character->GetStats()->GetDamage());
		if (m_health > 0)
		{
			ChangeState(new FlyHurt(this, character->GetStats()->GetDamage()));
			SoundManager::GetInstance()->Play("FlyHurt");
		}
		else
		{
			ChangeState(new FlyDeath(this));
			SoundManager::GetInstance()->Play("FlyDeath");

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

void FlyEntity::HitboxCollided(b2::UserData* _data)
{
	if (_data->name == "Chara")
	{
		CharaControl* character = (CharaControl*)_data->ptr;
		character->AddDamage(5);

		float angle = GetAngle(m_shapes[0]->GetPosition(), character->GetPos());

		character->SetKnockBack(angle, 10000);
	}
}

void FlyEntity::GenerateBaseAnimations(bool _background)
{
	if (m_animation != nullptr)
	{
		return;
	}

	sf::Sprite* sprite = m_sprite;
	sf::Texture* texture = nullptr;
	Animation* animation = nullptr;

	GetPositionFunc* func = new GetPositionFunc();
	*func = std::bind(&FlyEntity::GetScreenPos, this);

	texture = TextureManager::GetTexture("SPRITESHEET_MoucheDeplacement.png");
	//std::cout << texture << std::endl;
	m_animation = new Animation(CreateAnimation(*sprite, *texture, 2, 24.f, { 0,0,300,300 }));
	m_animators[1]->AddAnimation("Idle", m_animation);

	m_animators[1]->PlayAnimation(m_animation);

	m_animation = new Animation(CreateAnimation(*sprite, *texture, 1, 0.f, { 0,0,0,0 }));
	m_animators[1]->AddAnimation("None", m_animation);

	m_animation = new Animation(CreateAnimation(*sprite, *texture, 4, 4.f, { 0,300,300,300 }));
	m_animators[0]->AddAnimation("Idle", m_animation);

	texture = TextureManager::GetTexture("Fly_Attack_Spritesheet.png");

	m_attackAnim = new Animation(CreateAnimation(*sprite, *texture, 9, 10.f, { 0,300,300,300 }));
	m_attackAnim->loop = false;
	m_animators[0]->AddAnimation("BeginAttack", m_attackAnim);

	m_attackAnim = new Animation(CreateAnimation(*sprite, *texture, 4, 10.f, { 2400,300,300,300 }));
	m_attackAnim->loop = false;
	m_animators[0]->AddAnimation("Attack", m_attackAnim);

	texture = TextureManager::GetTexture("SPRITESHEET_MoucheMort.png");
	m_deathAnim = new Animation(CreateAnimation(*sprite, *texture, 6, 5.f, { 0,0,300,300 }));
	m_deathAnim->loop = false;
	m_animators[0]->AddAnimation("Death", m_deathAnim);
}
