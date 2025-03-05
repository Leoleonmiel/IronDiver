#include "EelHurt.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"

#include "EelIdle.h"

EelHurt::EelHurt(Entity* _owner, int _damage) : EntityState(_owner)
{
	m_damage = _damage;
}

void EelHurt::InitState()
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;

	m_elapsedTime = 0.f;
	m_owner->SetCurrentAnimation("Hurt");
	m_owner->FreezeBodies();
}

void EelHurt::Update()
{
	if (!m_hitboxDead)
	{
		m_owner->GetBody(1)->SetEnabled(false);
		m_hitboxDead = true;
	}
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	float knockbackPower = m_baseKnockback + (m_damageKnockback * (m_damage - 1));

	sf::Vector2f knockback = { knockbackPower * m_knockbackDirection, knockbackPower / 10.f };
	knockback *= 1 - (m_elapsedTime / m_duration);
	m_owner->Move(knockback);
}

void EelHurt::ExitState()
{
}

EntityState* EelHurt::GetNextState()
{
	if (m_elapsedTime < m_duration)
	{
		return nullptr;
	}

	return new EelIdle(m_owner);
}
