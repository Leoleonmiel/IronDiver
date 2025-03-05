#include "BossChaseEnd.h"
#include "common.h"
#include "Entity.h"
#include "EntityManager.h"
#include "BossChase.h"

BossChaseEnd::BossChaseEnd(Entity* _owner) : EntityState(_owner)
{
}

void BossChaseEnd::InitState()
{
	m_timer = m_duration;
}

void BossChaseEnd::Update()
{
	m_timer -= GameData::DeltaTime * GameData::GameSpeed;
	m_owner->Move({ 0, m_speed });

	if (m_timer <= 0.f)
	{
		EntityManager::GetInstance()->MarkForDeath(m_owner);
		BossChase::BossBeaten = true;
	}
}

void BossChaseEnd::ExitState()
{

}

EntityState* BossChaseEnd::GetNextState()
{
	return nullptr;
}
