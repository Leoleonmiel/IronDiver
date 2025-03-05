#include "BossShockwaveState.h"
#include "Entity.h"
#include "common.h"
#include "EntityManager.h"

BossShockwaveState::BossShockwaveState(Entity* _owner, bool _right) : EntityState(_owner)
{
	m_right = _right;
}

void BossShockwaveState::InitState()
{
	m_timer = m_lifetime;
}

void BossShockwaveState::Update()
{
	if (m_right)
	{
		m_owner->Move({ m_speed, 0.f });
	}
	else
	{
		m_owner->Move({ -m_speed, 0.f });
	}
	m_timer -= GameData::DeltaTime * GameData::GameSpeed;
	if (m_timer <= 0.f)
	{
		EntityManager::GetInstance()->MarkForDeath(m_owner);
	}
}

void BossShockwaveState::ExitState()
{

}

EntityState* BossShockwaveState::GetNextState()
{
	return nullptr;
}