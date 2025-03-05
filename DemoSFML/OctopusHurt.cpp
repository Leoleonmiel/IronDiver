#include "OctopusHurt.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"

#include "OctopusDeath.h"
#include "OctopusChase.h"
#include "OctopusIdleRise.h"

#include "TextureManager.h"
#include "Animation.h"
#include "Animator.h"

OctopusHurt::OctopusHurt(Entity* _owner, int _damage) : EntityState(_owner), m_damage(_damage)
{
}

void OctopusHurt::InitState()
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;
	
	Animation* animation;
	sf::Sprite* sprite = m_owner->GetSprite();
	sf::Texture texture = *TextureManager::GetTexture("Purple_Spritesheet.png");

	animation = new Animation(CreateAnimation(*sprite, texture, 1, 1.f, { 3000,0,500,500 }));

	m_owner->GetAnimator()[0]->PlayAnimation(animation);

	m_elapsedTime = 0.f;
}

void OctopusHurt::Update()
{
	float knockbackPower = m_baseKnockback + (m_damageKnockback * (m_damage - 1));

	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	sf::Vector2f knockback = { knockbackPower * m_knockbackDirection, knockbackPower / 10.f };
	knockback *= 1 - (m_elapsedTime / m_duration);
	m_owner->Move(knockback);
}

void OctopusHurt::ExitState()
{

}

EntityState* OctopusHurt::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();

	if (m_elapsedTime > m_duration)
	{
		if (m_owner->GetHealth() <= 0)
		{
			return new OctopusDeath(m_owner);
		}
		return new OctopusIdleRise(m_owner);
	}
	else if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange())
	{
		return new OctopusChase(m_owner);
	}
	return nullptr;
}