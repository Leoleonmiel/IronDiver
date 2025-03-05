#include "RayDeath.h"

#include "Entity.h"
#include "EntityManager.h"
#include "common.h"

RayDeath::RayDeath(Entity* _owner) : EntityState(_owner)
{
}

void RayDeath::InitState()
{
	m_owner->SetCurrentAnimation("Death");
	m_timer = 0.f;
	m_owner->SetGravity(1.f);
	m_owner->Move({ 0, -m_knockbackStrength });
}

void RayDeath::Update()
{
	m_timer += GameData::DeltaTime * GameData::GameSpeed;

	if (m_timer > m_duration)
	{
		if (m_timer < m_duration + m_shrinkDuration)
		{
			float shrink = 1 - ((m_timer - m_duration) / m_shrinkDuration);
			m_owner->GetSprite()->setColor(sf::Color(255, 255, 255, (sf::Uint8)(255 * shrink)));
		}
		else
		{
			EntityManager::GetInstance()->MarkForDeath(m_owner);
		}
	}
}

void RayDeath::ExitState()
{
}

EntityState* RayDeath::GetNextState()
{
	return nullptr;
}