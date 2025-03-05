#include "SpiderHurt.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"

#include "SpiderIdle.h"

SpiderHurt::SpiderHurt(Entity* _owner, int _damage) : EntityState(_owner)
{
	m_damage = _damage;
}

void SpiderHurt::InitState()
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;

	m_elapsedTime = 0.f;
	m_owner->SetCurrentAnimation("Hurt");
	m_owner->FreezeBodies();
}

void SpiderHurt::Update()
{
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	float knockbackPower = m_baseKnockback + (m_damageKnockback * (m_damage - 1));

	sf::Vector2f knockback = { knockbackPower * m_knockbackDirection, knockbackPower / 10.f };
	knockback *= 1 - (m_elapsedTime / m_duration);
	m_owner->Move(knockback);
}

void SpiderHurt::ExitState()
{
}

EntityState* SpiderHurt::GetNextState()
{
	if (m_elapsedTime < m_duration)
	{
		return nullptr;
	}

	return new SpiderIdle(m_owner);
}