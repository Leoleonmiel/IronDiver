#include "BossChaseWait.h"
#include "common.h"
#include "Entity.h"

BossChaseWait::BossChaseWait(Entity* _owner, float _duration, EntityState* _nextState) : EntityState(_owner)
{
	m_duration = _duration;
	m_nextState = _nextState;
}

void BossChaseWait::InitState()
{
	m_timer = m_duration;
	m_owner->SetCurrentAnimation("Idle");
}

void BossChaseWait::Update()
{
	m_timer -= GameData::DeltaTime * GameData::GameSpeed;
	m_owner->Move({ 0,0 });
}

void BossChaseWait::ExitState()
{

}

EntityState* BossChaseWait::GetNextState()
{
	if (m_duration > 0.f && m_timer <= 0.f && m_nextState != nullptr)
	{
		return m_nextState;
	}
	return nullptr;
}
