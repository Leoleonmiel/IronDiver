#include "MantisDeath.h"

#include "CharacterController.h"
#include "Entity.h"
#include "EntityManager.h"
#include "common.h"

MantisDeath::MantisDeath(Entity* _owner) : EntityState(_owner)
{
}

void MantisDeath::InitState()
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;
	m_timer = 0.f;
	m_owner->SetCurrentAnimation("Death");
}

void MantisDeath::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	float timeRatio;
	m_timer += dt;

	timeRatio = m_timer / m_duration;

	float knockbackPower = m_baseKnockback;

	sf::Vector2f knockback = { knockbackPower * m_knockbackDirection, knockbackPower / 10.f };
	knockback *= 1 - (m_elapsedTime / m_duration);
	m_owner->Move(knockback);

	if (m_timer >= m_duration)
	{
		EntityManager::GetInstance()->MarkForDeath(m_owner);
		return;
	}

	m_owner->SetRotation(m_angle);
}

void MantisDeath::ExitState()
{
}

EntityState* MantisDeath::GetNextState()
{
	return nullptr;
}
