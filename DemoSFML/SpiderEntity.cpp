#include "SpiderEntity.h"
#include "TextureManager.h"
#include "Animation.h"
#include "Animator.h"

#include "Box2D_Collision.h"
#include "Box2D_Shape.h"
#include "common.h"

#include "CharacterController.h"
#include "CameraManager.h"
#include "CharacterStats.h"
#include "SpiderStateInclude.h"
#include "EntityManager.h"
#include "ItemManager.h"
#include "SoundManager.h"
#include "ParticleManager.h"
#include "StandardParticles.h"
#include "TimeSpeedManager.h"

SpiderEntity::SpiderEntity(sf::Vector2f _pos, bool _background)
{
	b2Filter filter;
	GetPositionFunc* posFunc = nullptr;
	b2::Shape* shape = nullptr;
	b2::CollisionCallback* callback = nullptr;
	std::string hitboxName;

	filter.categoryBits = F_MONSTER;
	filter.maskBits = 0xFFFF - F_MONSTER;

	m_weight = 10.f;
	m_sightRange = 30.f;
	m_cowardly = false;
	m_flying = true;
	m_maxHealth = 3;
	m_health = m_maxHealth;
	m_position = _pos;
	m_invulnTime = 0.f;

	m_loots = new LootDef();

	m_bodyCount = 1;
	m_bodies = new b2Body * [1];
	m_shapes = new b2::Shape * [1];
	m_offsets = new sf::Vector2f[1];

	if (_background)
	{
		callback = NULL;
		hitboxName = "BackgroundEntity";
	}
	else
	{
		callback = new b2::CollisionCallback;
		*callback = std::bind(&SpiderEntity::HurtboxCollided, this, std::placeholders::_1);
		hitboxName = "Hurtbox";
	}

	shape = new b2::Rectangle(*GameData::World, b2_dynamicBody, m_position, { 100,100 }, hitboxName, (uintptr_t)(this), callback);
	m_shapes[0] = shape;

	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { 0,0 };
	shape->SetPhysics(10.f, 0.f, false, 0.f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);

	m_sprite = new sf::Sprite();
	m_sprite->setTextureRect({ 0, 0, 200, 200 });
	m_sprite->setOrigin(100, 100);

	posFunc = new GetPositionFunc();
	*posFunc = std::bind(&SpiderEntity::GetScreenPos, this);
	m_animators[0] = new Animator(posFunc);
	GenerateBaseAnimations(false);

	if (_background)
	{
		m_sprite->setScale(0.8f, 0.8f);
		m_sprite->setColor({ 150,150,150,255 });
	}
	else	
	{
		LootStats stats;
		stats.chance = 1.f;
		stats.min = 1;
		stats.max = 3;
		m_loots->onDeath[ItemManager::GetItem("Chromatica")] = stats;
	}

	m_state = new SpiderIdle(this);
}

SpiderEntity::SpiderEntity(const EntityDef& _def, sf::Vector2f _pos) : Entity(_def, _pos)
{
	m_state = new SpiderIdle(this);
}

void SpiderEntity::HurtboxCollided(b2::UserData* _data)
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
			ChangeState(new SpiderHurt(this, character->GetStats()->GetDamage()));
			SoundManager::GetInstance()->Play("SpiderHurt");
		}
		else
		{
			ChangeState(new SpiderDeath(this));
			SoundManager::GetInstance()->Play("SpiderDeath");

			

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

void SpiderEntity::GetLookDirection()
{
	if (m_bodies[0]->GetLinearVelocity().x == 0)
	{
		return;
	}
	
	if (m_bodies[0]->GetLinearVelocity().x > 0)
	{
		m_lookingRight = true;
	}
	else
	{
		m_lookingRight = false;
	}
}

void SpiderEntity::GenerateBaseAnimations(bool _background)
{
	sf::Sprite* sprite = m_sprite;
	sf::Texture* texture = TextureManager::GetTexture("Spider_Spritesheet.png");
	Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 13, 16.f, { 0,0,200,200 }));
	m_animators[0]->AddAnimation("Idle", animation);

	texture = TextureManager::GetTexture("Spider_Death_Spritesheet.png");
	animation = new Animation(CreateAnimation(*sprite, *texture, 17, 16.f, { 0,0,200,200 }));
	animation->loop = false;
	m_animators[0]->AddAnimation("Death", animation);

	animation = new Animation(CreateAnimation(*sprite, *texture, 1, 1.f, { 1400,0,200,200 }));
	animation->loop = false;
	m_animators[0]->AddAnimation("Hurt", animation);
}
