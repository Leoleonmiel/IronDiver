#include "ShrimpHurt.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"

#include "ShrimpIdle.h"

ShrimpHurt::ShrimpHurt(Entity* _owner, int _damage) : EntityState(_owner)
{
	m_damage = _damage;
}

void ShrimpHurt::InitState()
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;

	m_elapsedTime = 0.f;
	m_owner->SetCurrentAnimation("Hurt");
	m_owner->FreezeBodies();
}

void ShrimpHurt::Update()
{
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	float knockbackPower = m_baseKnockback + (m_damageKnockback * (m_damage - 1));

	sf::Vector2f knockback = { knockbackPower * m_knockbackDirection, knockbackPower / 10.f };
	knockback *= 1 - (m_elapsedTime / m_duration);
	m_owner->Move(knockback);
}

void ShrimpHurt::ExitState()
{
}

EntityState* ShrimpHurt::GetNextState()
{
	if (m_elapsedTime < m_duration)
	{
		return nullptr;
	}

	return new ShrimpIdle(m_owner);
}