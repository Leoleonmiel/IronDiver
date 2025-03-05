#include <queue>

#include "MantisHurt.h"
#include "MantisDeath.h"
#include "MantisIdle.h"

#include "SFML/Graphics.hpp"
#include "Entity.h"
#include "TextureManager.h"
#include "Animation.h"
#include "Animator.h"
#include "common.h"
#include "CharacterController.h"

MantisHurt::MantisHurt(Entity* _owner, int _damage) : EntityState(_owner)
{
	m_damage = _damage;
}

void MantisHurt::InitState()
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;

	m_elapsedTime = 0.f;
	sf::Sprite* sprite = m_owner->GetSprite();
	sf::Texture* texture = TextureManager::GetTexture("Mantis_Death_Spritesheet.png");
	Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 6, 1.f, { 0,0,150,150 }));
	std::queue<Animation*> animations;

	animations.push(animation);
	m_owner->GetAnimator()[0]->SetAnimationQueue(animations);
}

void MantisHurt::Update()
{
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	float knockbackPower = m_baseKnockback + (m_damageKnockback * (m_damage - 1));

	sf::Vector2f knockback = { knockbackPower * m_knockbackDirection, knockbackPower / 10.f };
	knockback *= 1 - (m_elapsedTime / m_duration);
	m_owner->Move(knockback);
}

void MantisHurt::ExitState()
{
}

EntityState* MantisHurt::GetNextState()
{
	if (m_elapsedTime < m_duration)
	{
		return nullptr;
	}

	if (m_owner->GetHealth() > 0)
	{
		return new MantisIdle(m_owner);
	}

	return new MantisDeath(m_owner);
}
