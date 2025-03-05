#include "JellyfishEntity.h"
#include <queue>

#include "Animator.h"
#include "Animation.h"
#include "TextureManager.h"
#include "EntityStateInclude.h"
#include "CharacterController.h"
#include "CameraManager.h"
#include "CharacterStats.h"
#include "ItemManager.h"
#include "JellyfishStateInclude.h"
#include "SoundManager.h"

#include "Box2D_Collision.h"
#include "Box2D_Shape.h"
#include "EntityManager.h"
#include "TimeSpeedManager.h"
#include "ParticleManager.h"
#include "StandardParticles.h"

JellyfishEntity::~JellyfishEntity()
{
}

JellyfishEntity::JellyfishEntity(sf::Vector2f _pos)
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
	m_bodies = new b2Body * [1];
	m_shapes = new b2::Shape * [1];
	m_offsets = new sf::Vector2f[1];

	callback = new b2::CollisionCallback;
	*callback = std::bind(&JellyfishEntity::HurtboxCollided, this, std::placeholders::_1);

	shape = new b2::Rectangle(*GameData::World, b2_dynamicBody, m_position, { 148,148 }, "Hurtbox", (uintptr_t)(this), callback);
	m_shapes[0] = shape;

	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { 0,24 };
	shape->SetPhysics(10.f, 0.f, false, 1.2f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);

	m_sprite = new sf::Sprite();
	m_sprite->setTextureRect({ 0, 0, 400, 400 });
	m_sprite->setOrigin(200, 200);

	posFunc = new GetPositionFunc();
	*posFunc = std::bind(&JellyfishEntity::GetScreenPos, this);
	m_animators[0] = new Animator(posFunc);

	LootStats stats;
	stats.chance = 1.f;
	stats.min = 1;
	stats.max = 3;
	m_loots->onDeath[ItemManager::GetItem("Rock")] = stats;

	GenerateBaseAnimations(false);

	m_state = new JellyfishIdleRise(this);
}

JellyfishEntity::JellyfishEntity(const EntityDef& _def, sf::Vector2f _pos) : Entity(_def, _pos)
{
	m_state = new JellyfishIdleRise(this);
}

void JellyfishEntity::HurtboxCollided(b2::UserData* _data)
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
			ChangeState(new JellyfishHurt(this, character->GetStats()->GetDamage()));
		}
		else
		{
			ChangeState(new JellyfishDeath(this));
			SoundManager::GetInstance()->Play("JellyfishDeath");

			

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

void JellyfishEntity::GenerateBaseAnimations(bool _background)
{
	if (m_animation != nullptr)
	{
		return;
	}

	std::queue<Animation*> animations;
	sf::Sprite* sprite = m_sprite;
	sf::Texture texture;
	Animation* animation = NULL;

	GetPositionFunc* func = new GetPositionFunc();
	*func = std::bind(&JellyfishEntity::GetScreenPos, this);

	texture = *TextureManager::GetTexture("Jellyfish_Spritesheet.png");
	m_animation = new Animation(CreateAnimation(*sprite, texture, 20, 12.f, { 0,0,400,400 }));
	m_animators[0]->AddAnimation("Idle", m_animation);

	m_freezeFrame = new Animation(CreateAnimation(*sprite, texture, 10, 6.f, { 4000,0,400,400 }));
	m_freezeFrame->loop = false;
	m_animators[0]->AddAnimation("FreezeAnim", m_freezeFrame);

	texture = *TextureManager::GetTexture("Jellyfish_Death_Spritesheet.png");
	m_deathAnim = new Animation(CreateAnimation(*sprite, texture, 12, 9.f, { 2000,0,400,400 }));
	m_deathAnim->loop = false;
	m_animators[0]->AddAnimation("Death", m_deathAnim);

	animations.push(m_animation);

	m_animators[0]->SetAnimationQueue(animations);
}
