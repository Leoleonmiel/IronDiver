#pragma once
#include <iostream>
#include <stack>
#include <unordered_map>
#include <type_traits>

#include "SFML/Graphics.hpp"
#include "box2D/box2D.h"


//#define MONSTER_FILTER (uint16)0b0000000000000010

class EntityState;
struct Animation;
namespace b2 { struct UserData; }
namespace b2 { class Shape; }
class Animator;

enum HitboxShape
{
	RECTANGLE,
	CIRCLE,
	POLYGON,
	POINT_HITBOX,
	SENSOR
};

struct Item;

struct LootStats
{
	float chance;
	unsigned int min;
	unsigned int max;
};

struct LootDef
{
	std::unordered_map<Item*, LootStats> onDeath;
	std::unordered_map<Item*, LootStats> onThreshold;

	float threshold = 1.f;
};

enum HitboxType
{
	HITBOX,
	HURTBOX
};

struct EntityHitboxDef
{
	HitboxShape shape = RECTANGLE;
	b2BodyType bodyType = b2_staticBody;
	sf::Vector2f offset;

	float radius = 0.f;
	sf::Vector2f size;
	b2Vec2* vertices = nullptr;
	int vertexCount = 0;;

	bool sensor = false;
	bool active = false;
	bool fixedRotation = false;
	float bounce = 0.f;

	HitboxType type = HURTBOX;
};

struct EntityDef
{
	float weight = 0.f;
	float sightRange = 0.f;

	bool cowardly = false;
	bool flying = false;

	int health = 0;

	std::vector<EntityHitboxDef> hitboxes;
	LootDef* loots = nullptr;
};



class Entity
{
protected:
	b2Body** m_bodies = nullptr;
	b2::Shape** m_shapes = nullptr;
	sf::Vector2f* m_offsets = nullptr;

	std::map<std::string, sf::Sprite*> m_animatedSprites;
	std::map<std::string, Animation*> m_animations;

	LootDef* m_loots;
	float m_invulnTime = 0.f;
	int m_damageThreshold = 0;

	EntityState* m_state = nullptr;

	sf::Vector2f m_position;
	sf::Vector2f m_scale = { 1,1 };

	float m_weight = 0.f;
	float m_sightRange = 0.f;
	float m_rotation = 0.f;

	int m_bodyCount = 0;
	int m_maxHealth = 0;
	int m_health = 0;

	bool m_cowardly = false;
	bool m_flying = false;
	bool m_lookingRight = true;
	bool m_canGrab = true;

	Animation* m_currentAnimation = nullptr;
	std::vector<Animator*> m_animators;
	sf::Sprite* m_sprite;

	void SetDefaultState(EntityState* _state);
	
public:

	Entity();
	Entity(const EntityDef& _def, sf::Vector2f _pos = sf::Vector2f{ 0,0 });
	virtual ~Entity();

	void ChangeState(EntityState* _state);

#pragma region Virtual
	virtual void HurtboxCollided(b2::UserData* _data);
	virtual void HitboxCollided(b2::UserData* _data);
	virtual void Display();
	virtual void Update();
	virtual void GenerateBaseAnimations(bool _background);
	virtual void Move(sf::Vector2f);
	virtual void GetLookDirection();
#pragma endregion

	void FreezeBodies();

#pragma region Getters
	bool IsCowardly() const { return m_cowardly; }
	float SightRange() const { return m_sightRange; }
	bool IsFlying() const { return m_flying; }
	sf::Vector2f GetScreenPos();
	Animation* GetAnimation(std::string _name);
	b2Vec2 GetPos() { return m_bodies[0]->GetPosition(); }
	std::vector<Animator*>& GetAnimator() { return m_animators; }
	b2::Shape* GetCoreShape() { return m_shapes[0]; }
	b2::Shape* GetShape(int _index) { return m_shapes[_index]; }
	b2Body* GetBody(int _index) { return m_bodies[_index]; }
	sf::Sprite* GetSprite() { return m_sprite; }
	int GetHealth() { return m_health; }
	int GetMaxHealth() { return m_maxHealth; }
	float GetRotation() { return m_rotation; }
	bool isLookingRight() { return m_lookingRight; }
	bool GetIsGrabbed();
	EntityState* GetState() { return m_state; }
#pragma endregion

#pragma region Setters
	void SetPos(sf::Vector2f _pos);
	void SetCurrentAnimation(std::string _name);
	void SetRotation(float _rotation) { m_rotation = _rotation; }
	void SetGravity(float _gravity) { m_bodies[0]->SetGravityScale(_gravity); }
	void SetScale(sf::Vector2f _scale) { m_scale = _scale; }
	void SetFixedRotation(bool _fixedRotation) { m_bodies[0]->SetFixedRotation(_fixedRotation); }
	void SetCanGrab(bool _canGrab) { m_canGrab = _canGrab; }
	void SetHitboxEnabled(int _index, bool _enabled);
	void SetState(EntityState* _state);
#pragma endregion

#pragma region Animations
	void GenerateNewAnimation(std::string _path, std::string _name, int _frameCount, float _animSpeed, sf::IntRect _rect);
#pragma endregion

#pragma region Damage
	void TakeDamage(int _damage);
	void TakeKnockback(sf::Vector2f _knockback);
#pragma endregion
};

template<class T>
static Entity* GenerateEntity(const EntityDef& _def, sf::Vector2f _pos = sf::Vector2f{ 0,0 })
{
	static_assert(std::is_base_of<Entity, T>::value, "Class must derive from Entity");
	Entity* entity = new T(_def, _pos);
	entity->GenerateBaseAnimations();
	/*if (entity->m_flying)
	{
		entity->SetCurrentAnimation("fly");
	}
	else
	{
		entity->SetDefaultState(new EntityIdleGeneric(entity));
	}*/
	return entity;
}

template<class T>
static Entity* GenerateEntity(sf::Vector2f _pos = sf::Vector2f{ 0,0 })
{
	static_assert(std::is_base_of<Entity, T>::value, "Class must derive from Entity");
	Entity* entity = new T(_pos);
	entity->GenerateBaseAnimations(false);
	return entity;
}

