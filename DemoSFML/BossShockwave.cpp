#include "BossShockwave.h"
#include "EntityStateInclude.h"
#include "Box2D_Shape.h"
#include "Box2D_Collision.h"
#include "common.h"
#include "CharacterController.h"
#include "Animator.h"
#include "TextureManager.h"
#include "Animation.h"

BossShockwave::BossShockwave(bool _lookingRight, sf::Vector2f _pos)
{
	b2Filter filter;
	b2::Shape* shape;
	b2::CollisionCallback* callback;

	filter.categoryBits = F_MONSTER;
	filter.maskBits = 0xFFFF - F_MONSTER;

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
	m_bodies = new b2Body * [m_bodyCount];
	m_shapes = new b2::Shape * [m_bodyCount];
	m_offsets = new sf::Vector2f[m_bodyCount];

	callback = new b2::CollisionCallback();
	*callback = std::bind(&BossShockwave::HitboxCollided, this, std::placeholders::_1);

	shape = new b2::Rectangle(*GameData::World, b2_dynamicBody, m_position, { 480,150 }, "ShockwaveHitbox", (uintptr_t)(this), callback);
	m_shapes[0] = shape;
	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { 0,0 };
	shape->SetPhysics(1.0f, 0.f, true, 0.f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);
	m_lookingRight = _lookingRight;

	m_state = new EntityIdleGeneric(this);

	GenerateBaseAnimations(false);
}

void BossShockwave::HitboxCollided(b2::UserData* _data)
{
	if (_data->name == "Chara")
	{
		auto& chara = GameData::CharaCtrl;
		if (!chara->GetIsInvincible())
		{
			chara->AddDamage(10);
			chara->SetKnockBack(m_bodies[0]->GetLinearVelocity().x > 0 ? 0.f : 180.f, 3000.f);
		}
	}
}

void BossShockwave::GenerateBaseAnimations(bool _background)
{
	GetPositionFunc* func = new GetPositionFunc();
	*func = [this]()->sf::Vector2f {return GetScreenPos(); };
	m_animators.push_back(new Animator(func));
	
	sf::Sprite* sprite = m_sprite;
	//sprite->setOrigin(480/2, 390/2);
	sf::Texture* texture = TextureManager::GetTexture("Shockwave_Spritesheet.png");
	//Animation* anim = new Animation(CreateAnimation(*sprite, *texture, 15, 6.f, { 0,0,1600,800 }));
	Animation* anim = new Animation(CreateAnimation(*sprite, *texture, 15, 12.f, { 0,0,480,390 }));
	anim->loop = false;

	m_animators[0]->AddAnimation("Idle", anim);
}

//void BossShockwave::Display()
//{
//	sf::RectangleShape rect;
//	rect.setSize({ 480,150 });
//	rect.setFillColor(sf::Color(200, 0, 0, 100));
//	rect.setOrigin(240, 75);
//	rect.setPosition(GetScreenPos());
//	GameData::Window->draw(rect);
//}
