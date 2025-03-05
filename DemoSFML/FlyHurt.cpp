#include "FlyHurt.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"

#include "FlyDeath.h"
#include "FlyChase.h"
#include "FlyIdle.h"

#include "TextureManager.h"
#include "Animation.h"
#include "Animator.h"

FlyHurt::FlyHurt(Entity* _owner, int _damage) : EntityState(_owner)
{
	m_damage = _damage;
}

void FlyHurt::InitState()
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;

	Animation* animation;
	sf::Sprite* sprite = m_owner->GetSprite();
	sf::Texture texture = *TextureManager::GetTexture("Fly_Attack_Spritesheet.png");

	animation = new Animation(CreateAnimation(*sprite, texture, 4, 2.f, { 900,300,300,300 }));

	m_owner->GetAnimator()[0]->PlayAnimation(animation);

	m_elapsedTime = 0.f;
}

void FlyHurt::Update()
{
	if (!m_hitboxDead)
	{
		m_owner->GetBody(1)->SetEnabled(false);
		m_hitboxDead = true;
	}
	float knockbackPower = m_baseKnockback + (m_damageKnockback * (m_damage - 1));

	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	sf::Vector2f knockback = { knockbackPower * m_knockbackDirection, knockbackPower / 10.f };
	knockback *= 1 - (m_elapsedTime / m_duration);
	m_owner->Move(knockback);
}

void FlyHurt::ExitState()
{
}

EntityState* FlyHurt::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (m_elapsedTime > m_duration)
	{
		if (m_owner->GetHealth() <= 0)
		{
			return new FlyDeath(m_owner);
		}
		else if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange())
		{
			return new FlyChase(m_owner);
		}
		return new FlyIdle(m_owner);
	}
	return nullptr;
}
