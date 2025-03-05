#include "Entity.h"
#include "common.h"

#include "EntityManager.h"
#include "EntityStateInclude.h"
#include "EntityIdleGeneric.h"

#include "Box2D_Collision.h"
#include "Box2D_World.h"
#include "Box2D_Shape.h"

#include "CharacterController.h"
#include "GrabHook.h"
#include "CharacterStats.h"
#include "CameraManager.h"
#include "ItemManager.h"
#include "Collectable.h"
#include "Animation.h"
#include "Animator.h"
#include "Box2D_Collision.h"

#pragma region private
void Entity::SetDefaultState(EntityState* _state)
{
	m_state = _state;
	_state->InitState();
}

Entity::Entity()
{
	m_shapes = nullptr;
	m_offsets = nullptr;

	m_loots = new LootDef;

	GetPositionFunc* func = new GetPositionFunc;
	*func = std::bind(&Entity::GetScreenPos, this);
	m_animators.push_back(new Animator(func));

	m_sprite = new sf::Sprite;
	m_sprite->setOrigin(16, 16);
}

Entity::Entity(const EntityDef& _def, sf::Vector2f _pos)
{
	m_weight = _def.weight;
	m_sightRange = _def.sightRange;

	m_cowardly = _def.cowardly;
	m_flying = _def.flying;

	m_position = _pos;
	m_bodyCount = (int)_def.hitboxes.size();
	m_bodies = new b2Body * [m_bodyCount];

	m_maxHealth = _def.health;
	m_health = m_maxHealth;

	m_invulnTime = 0.f;
	m_loots = _def.loots;

	m_shapes = new b2::Shape * [m_bodyCount];
	m_offsets = new sf::Vector2f[m_bodyCount];

	//Set-Up Animator
	GetPositionFunc* func = new GetPositionFunc;
	*func = std::bind(&Entity::GetScreenPos, this);
	m_animators.push_back(new Animator(func));
	m_sprite = new sf::Sprite;
	m_sprite->setOrigin(16, 16);

	for (int i = 0; i < m_bodyCount; i++)
	{
		b2::Shape* shape = nullptr;
		const auto& hitbox = _def.hitboxes[i];
		b2::CollisionCallback* callback;
		switch (hitbox.type)
		{
		case HURTBOX:
			callback = new b2::CollisionCallback;
			*callback = std::bind(&Entity::HurtboxCollided, this, std::placeholders::_1);
			break;
		default:
			callback = new b2::CollisionCallback;
			*callback = std::bind(&Entity::HitboxCollided, this, std::placeholders::_1);
			break;
		}

		switch (hitbox.shape)
		{
		case HitboxShape::CIRCLE:
			shape = new b2::Circle(*GameData::World, hitbox.bodyType, _pos, hitbox.radius, hitbox.offset, "Entity", (uintptr_t)this, callback);
			break;
		case HitboxShape::RECTANGLE:
			shape = new b2::Rectangle(*GameData::World, hitbox.bodyType, _pos, hitbox.size, "Entity", (uintptr_t)this, callback);
			break;
		default:
			break;
		}

		m_shapes[i] = shape;

		if (shape != nullptr)
		{
			m_bodies[i] = shape->GetBody();
			m_offsets[i] = hitbox.offset;
			shape->SetPhysics(m_weight, 0.f, hitbox.sensor, hitbox.bounce);
			m_bodies[i]->SetGravityScale(0);
			m_bodies[i]->SetEnabled(hitbox.active);
			b2Filter filter;
			filter.categoryBits = F_MONSTER;
			filter.maskBits = 0xFFFF - F_MONSTER;
			m_bodies[i]->GetFixtureList()[0].SetFilterData(filter);
			m_bodies[i]->SetFixedRotation(hitbox.fixedRotation);
		}
	}
}
#pragma endregion

Entity::~Entity()
{
	float randf = 0.f;
	if (m_loots != nullptr)
	{
		for (auto it = m_loots->onDeath.begin(); it != m_loots->onDeath.end(); it++)
		{
			randf = rand() / (float)RAND_MAX;
			if (randf < (*it).second.chance)
			{
				int count;
				if ((*it).second.max - (*it).second.min)
				{
					count = (*it).second.min + rand() % ((*it).second.max - (*it).second.min);
				}
				else
				{
					count = (*it).second.min;
				}
				Item* item = (*it).first;
				for (int i = 0; i < count; i++)
				{
					GameData::Collectables->push_back(new Collectable(EntityDef{ 0.f, 0.f, false, false, 0, std::vector<EntityHitboxDef>(), nullptr }, item, GetScreenPos()));
				}
			}
		}
	}

	b2::World& world = *GameData::World;
	for (int i = 0; i < m_bodyCount; i++)
	{
		if (m_shapes[i] != nullptr)
		{
			delete m_shapes[i];
			m_shapes[i] = nullptr;
		}

		if (m_bodies[i] != nullptr)
		{
			((b2::UserData*)m_bodies[i]->GetFixtureList()[0].GetUserData().pointer)->callback = nullptr;
		}
	}

	delete[] m_offsets;
	delete m_state;
	delete m_currentAnimation;
	delete m_loots;
}

void Entity::ChangeState(EntityState* _state)
{
	m_state->ExitState();
	delete(m_state);
	m_state = _state;
	m_state->InitState();
}

#pragma region Virtual
void Entity::HurtboxCollided(b2::UserData* _data)
{
	if (_data->name == "Attack" && m_health > 0 && m_invulnTime <= 0.f)
	{
		CharaControl* character = (CharaControl*)_data->ptr;
		sf::Vector2f playerPos = character->GetPos();
		sf::Vector2f entityPos = GetScreenPos();
		sf::Vector2f path = entityPos - playerPos;
		path /= std::sqrt(DistanceSquared(path, { 0,0 }));
		TakeKnockback({ path * 500.f });
		TakeDamage(character->GetStats()->GetDamage());

		m_invulnTime = 0.5f;

		if (m_health <= 0)
		{

			//EntityManager::GetInstance()->MarkForDeath(this);
		}
	}
}

void Entity::HitboxCollided(b2::UserData* _data)
{
	if (_data->name == "Chara")
	{
		CharaControl* character = (CharaControl*)_data->ptr;
		character->AddDamage(5);

		float angle = GetAngle(m_shapes[1]->GetPosition(), character->GetPos());

		character->SetKnockBack(angle, 100.f);
	}
}

void Entity::Display()
{
	sf::Vector2f path = GetScreenPos() - GameData::CharaCtrl->GetPos();
	if (m_lookingRight)
	{
		m_sprite->setScale(-m_scale.x, m_scale.y);
	}
	else
	{
		m_sprite->setScale(m_scale.x, m_scale.y);
	}

	m_sprite->setRotation(m_rotation + m_bodies[0]->GetAngle() * 180.f / -3.14159f);

	for (Animator* animator : m_animators)
	{
		animator->Display(*GameData::Window);
	}
}

void Entity::Update()
{
	EntityState* state = nullptr;
	if (m_state != nullptr)
	{
		m_state->Update();
	}
	
	state = m_state->GetNextState();
	if (state != nullptr)
	{
		ChangeState(state);
	}
	for (int i = 1; i < m_bodyCount; i++)
	{
		m_bodies[i]->SetTransform(Screen2World(GetScreenPos() + m_offsets[i]), 0);
	}

	if (m_invulnTime > 0.f)
	{
		m_invulnTime -= GameData::DeltaTime * 0.2f;
	}

	GetLookDirection();
}

void Entity::GenerateBaseAnimations(bool _background)
{
}

void Entity::Move(sf::Vector2f _move)
{
	m_position += _move;
	for (int i = 0; i < m_bodyCount; i++)
	{
		m_bodies[i]->SetLinearVelocity(Screen2World(_move));
	}
}
#pragma endregion

void Entity::FreezeBodies()
{
	for (int i = 0; i < m_bodyCount; i++)
	{
		m_bodies[i]->SetAngularVelocity(0);
		m_bodies[i]->SetLinearVelocity({ 0,0 });
		m_bodies[i]->ResetMassData();
	}
}

#pragma region Getters
void Entity::GetLookDirection()
{
	sf::Vector2f path = World2Screen(m_bodies[0]->GetPosition(), { 0,0 }) - GameData::CharaCtrl->GetPos();
	if (path.x < 0)
	{
		m_lookingRight = true;
	}
	else
	{
		m_lookingRight = false;
	}
}

sf::Vector2f Entity::GetScreenPos()
{
	sf::Vector2f offset = m_lookingRight ? sf::Vector2f(-m_offsets[0].x, m_offsets[0].y) : m_offsets[0];
	return World2Screen(m_bodies[0]->GetPosition(), { 0,0 }) + offset;
}

Animation* Entity::GetAnimation(std::string _name)
{
	Animation* animation = nullptr;
	m_animators[0]->TryGetAnimation(_name, animation);
	return animation;
}

bool Entity::GetIsGrabbed()
{
	return m_shapes[0]->GetGrabbed();
}
#pragma endregion

#pragma region Setters

void Entity::SetPos(sf::Vector2f _pos)
{
	sf::Vector2f offset = m_position - _pos;
	m_position = _pos;

	for (int i = 0; i < m_bodyCount; i++)
	{
		m_bodies[i]->SetTransform(Screen2World(_pos - (m_lookingRight ? sf::Vector2f{ -m_offsets[i].x, m_offsets[i].y} : m_offsets[i])), m_bodies[i]->GetAngle());
	}
}

void Entity::SetCurrentAnimation(std::string _name)
{
	m_animators[0]->TrySetCurrentAnimation(_name);
}
void Entity::SetHitboxEnabled(int _index, bool _enabled)
{
	m_bodies[_index]->GetFixtureList()[0].SetSensor(_enabled);
}
void Entity::SetState(EntityState* _state)
{
	m_state->ExitState(); 
	delete m_state; 
	m_state = _state; 
	m_state->InitState();
}
#pragma endregion

#pragma region Animations
void Entity::GenerateNewAnimation(std::string _path, std::string _name, int _frameCount, float _animSpeed, sf::IntRect _rect)
{
	if (m_animators[0]->TryGetAnimation(_name))
	{
		return;
	}
	sf::Texture texture;
	texture.loadFromFile(_path);
	Animation* anim = new Animation(CreateAnimation(*m_sprite, texture, _frameCount, _animSpeed, _rect));
	m_animators[0]->AddAnimation(_name, anim);
}

#pragma endregion

#pragma region Damage
void Entity::TakeDamage(int _damage)
{
	float randf;

	m_health -= _damage;
	m_damageThreshold += _damage;

	while (m_damageThreshold >= m_loots->threshold && m_loots->onThreshold.size() > 0)
	{
		for (auto it = m_loots->onThreshold.begin(); it != m_loots->onThreshold.end(); it++)
		{
			randf = rand() / (float)RAND_MAX;
			if (randf < (*it).second.chance)
			{
				int count;
				if ((*it).second.max - (*it).second.min)
				{
					count = (*it).second.min + rand() % ((*it).second.max - (*it).second.min);
				}
				else
				{
					count = (*it).second.min;
				}
				Item* item = (*it).first;
				for (int i = 0; i < count; i++)
				{
					GameData::Collectables->push_back(new Collectable(EntityDef{ 0.f, 0.f, false, false, 0, std::vector<EntityHitboxDef>(), nullptr }, item, GetScreenPos()));
				}
			}
		}
		m_damageThreshold -= (int)m_loots->threshold;
	}

	if (m_health <= 0)
	{
		SetCanGrab(false);
		GetCoreShape()->SetName("Death");
		if (GetIsGrabbed())
		{
			GameData::CharaCtrl->GetGrapHook()->BodyGrabbedDestroyed();
		}
	}

	return;
}

void Entity::TakeKnockback(sf::Vector2f _knockback)
{
	ChangeState(new EntityKnockbackFlying(this));
	m_bodies[0]->SetLinearVelocity(Screen2World(_knockback));
}
#pragma endregion