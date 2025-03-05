#include "OctopusEntity.h"
#include <queue>
#include "Animation.h"
#include "Animator.h"
#include "TextureManager.h"
#include "StandardParticles.h"
#include "ParticleManager.h"
#include "common.h"
#include "EntityStateInclude.h"
#include "Box2D_Collision.h"
#include "CharacterController.h"
#include "CharacterStats.h"
#include "CameraManager.h"
#include "ItemManager.h"
#include "SoundManager.h"

#include "Box2D_World.h"
#include "Box2D_Shape.h"

#include "OctopusStateInclude.h"
#include "OctopusBackgroundIdleRise.h"
#include "EntityManager.h"
#include "TimeSpeedManager.h"
#include "ParticleManager.h"
#include "StandardParticles.h"

OctopusEntity::OctopusEntity(const EntityDef& _def, sf::Vector2f _pos) : Entity(_def, _pos)
{
	m_state = new OctopusIdleRise(this);
}

void OctopusEntity::HitboxCollided(b2::UserData* _data)
{
	if (_data->name == "Chara")
	{
		CharaControl* character = (CharaControl*)_data->ptr;
		character->AddDamage(5);

		float angle = GetAngle(m_shapes[1]->GetPosition(), character->GetPos());

		character->SetKnockBack(angle, 100.f);
		m_playerHit = true;
	}
}

void OctopusEntity::HurtboxCollided(b2::UserData* _data)
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
			ChangeState(new OctopusHurt(this, character->GetStats()->GetDamage()));
			SoundManager::GetInstance()->Play("OctopusHurt");
		}
		else
		{
			ChangeState(new OctopusDeath(this));
			SoundManager::GetInstance()->Play("OctopusDeath");

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

		m_state = new OctopusIdleRise(this);

		m_invulnTime = 0.5f;
	}
}

OctopusEntity::~OctopusEntity()
{
	const int particleCount = 10;
	ParticleBase** particles = new ParticleBase * [particleCount];
	float randomAngle = 0;
	float randomSpeed = 0;
	float randomLifeTime = 0;

	for (int i = 0; i < particleCount; i++)
	{
		randomAngle = ((float)rand() / RAND_MAX) * 0.25f;
		randomAngle -= 0.625f;
		randomAngle *= 3.14f;

		randomSpeed = ((float)rand() / RAND_MAX) * 350.f;
		randomSpeed += 50.f;

		randomLifeTime = ((float)rand() / RAND_MAX) * 2.5f;
		randomLifeTime += 1.5f;
		
		particles[i] = new StandardParticle(randomAngle, randomSpeed, randomLifeTime, World2Screen(GetPos(), {0,0}));
		((StandardParticle*)particles[i])->SetGravity(750.f);
	}

	ParticleManager::AddParticle(particles, particleCount);
}

OctopusEntity::OctopusEntity(sf::Vector2f _pos, bool _background)
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

	m_bodyCount = _background ? 1 : 2;
	m_bodies = new b2Body *[m_bodyCount];
	m_shapes = new b2::Shape*[m_bodyCount];
	m_offsets = new sf::Vector2f[m_bodyCount];

	if (_background)
	{
		callback = NULL;
		hitboxName = "BackgroundEntity";
	}
	else
	{
		callback = new b2::CollisionCallback;
		*callback = std::bind(&OctopusEntity::HurtboxCollided, this, std::placeholders::_1);
		hitboxName = "Hurtbox";
	}

	shape = new b2::Rectangle(*GameData::World, b2_dynamicBody, m_position, { 192,32 }, hitboxName, (uintptr_t)(this), callback);
	m_shapes[0] = shape;

	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { 0,96 };
	shape->SetPhysics(10.f, 0.f, _background ? true : false, 0.f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);
	
	if (!_background)
	{
		callback = new b2::CollisionCallback;
		*callback = std::bind(&OctopusEntity::HitboxCollided, this, std::placeholders::_1);

		shape = new b2::Circle(*GameData::World, b2_kinematicBody, m_position, 128.f, { 0,0 }, "Hitbox", (uintptr_t)(this), callback);
		m_shapes[1] = shape;

		m_bodies[1] = shape->GetBody();
		m_offsets[1] = { 0,0 };
		shape->SetPhysics(0.f, 100.f, _background ? true : false, 0.f);
		m_bodies[1]->SetGravityScale(0.f);
		m_bodies[1]->GetFixtureList()[0].SetFilterData(filter);
		m_bodies[1]->SetFixedRotation(true);
		m_bodies[1]->SetEnabled(false);
	}

	m_sprite = new sf::Sprite();
	m_sprite->setTextureRect({ 0, 0, 500, 500 });
	m_sprite->setOrigin(250, 250);

	posFunc = new GetPositionFunc();
	*posFunc = std::bind(&OctopusEntity::GetScreenPos, this);
	m_animators[0] = new Animator(posFunc);

	GenerateBaseAnimations(_background);

	if (_background)
	{
		/*m_scale = {0.5f, 0.5f };
		m_sprite->setColor({ 150,200,150,200 });*/
		/*m_sprite->setOrigin(125, 125);
		m_state = new OctopusBackgroundIdleRise(this);*/
	}
	else
	{
		LootStats stats;
		stats.chance = 1.f;
		stats.min = 1;
		stats.max = 3;
		m_loots->onDeath[ItemManager::GetItem("Etherium")] = stats;

		m_state = new OctopusIdleRise(this);
	}

	m_state = new OctopusIdleRise(this);
}

void OctopusEntity::GenerateBaseAnimations(bool _background)
{
	_background = false;

	std::queue<Animation*> animations;
	sf::Sprite* sprite = m_sprite;
	sf::Texture texture;
	Animation* animation = NULL;
	int frameSize = _background ? 250 : 500;

	GetPositionFunc* func = new GetPositionFunc();
	*func = std::bind(&OctopusEntity::GetScreenPos, this);

	//texture = _background ? *TextureManager::GetTexture("Octopus_Background_Spritesheet.png") : *TextureManager::GetTexture("Octopus_Spritesheet.png");
	//m_animation = new Animation(CreateAnimation(*sprite, texture, 2, 12.f, { 0,0,frameSize,frameSize }));
	//m_animation->loop = false;
	//m_animators[0]->AddAnimation("RiseAnim", m_animation);

	texture = *TextureManager::GetTexture("Octopus_Spritesheet.png");
	m_riseAnim = new Animation(CreateAnimation(*sprite, texture, 2, 12.f, { 0,0,frameSize,frameSize }));
	m_riseAnim->loop = false;
	m_animators[0]->AddAnimation("RiseAnim", m_riseAnim);

	m_freezeFrame = new Animation(CreateAnimation(*sprite, texture, 6, 6.f, { 2 * frameSize,0,frameSize,frameSize }));
	m_freezeFrame->loop = false;
	m_animators[0]->AddAnimation("FreezeAnim", m_freezeFrame);

	texture = *TextureManager::GetTexture("Octopus_Death_Spritesheet.png");
	m_deathAnim = new Animation(CreateAnimation(*sprite, texture, 7, 10.f, { 0,0,1000,1000 }));
	m_deathAnim->loop = false;
	m_animators[0]->AddAnimation("Death", m_deathAnim);

	animations.push(m_riseAnim);

	texture = *TextureManager::GetTexture("Octopus_Attack_Spritesheet.png");
	m_beginAttackAnim = new Animation(CreateAnimation(*sprite, texture, 5, 12.f, { 0,0,500,500 }));
	m_beginAttackAnim->loop = false;
	m_animators[0]->AddAnimation("BeginAttack", m_beginAttackAnim);

	m_attackLoopAnim = new Animation(CreateAnimation(*sprite, texture, 3, 24.f, { 2500,0,500,500 }));
	m_animators[0]->AddAnimation("AttackLoop", m_attackLoopAnim);

	m_animators[0]->SetAnimationQueue(animations);
}

void OctopusEntity::Update()
{
	Entity::Update();

	if (m_playerHit)
	{
		m_bodies[1]->SetEnabled(false);
		m_playerHit = false;
	}
}
