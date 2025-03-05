#include "RayHurt.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"

#include "RayIdle.h"

RayHurt::RayHurt(Entity* _owner, int _damage) : EntityState(_owner)
{
	m_damage = _damage;
}

void RayHurt::InitState()
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;

	m_elapsedTime = 0.f;
	m_owner->SetCurrentAnimation("Hurt");
	m_owner->FreezeBodies();
}

void RayHurt::Update()
{
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	float knockbackPower = m_baseKnockback + (m_damageKnockback * (m_damage - 1));

	sf::Vector2f knockback = { knockbackPower * m_knockbackDirection, knockbackPower / 10.f };
	knockback *= 1 - (m_elapsedTime / m_duration);
	m_owner->Move(knockback);
}

void RayHurt::ExitState()
{
}

EntityState* RayHurt::GetNextState()
{
	if (m_elapsedTime < m_duration)
	{
		return nullptr;
	}

	return new RayIdle(m_owner, m_knockbackDirection == 1 ? true : false, 5.f);
}