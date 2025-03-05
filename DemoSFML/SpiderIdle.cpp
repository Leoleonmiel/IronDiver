#include "SpiderIdle.h"
#include "Entity.h"
#include "common.h"
#include "SpiderIdleMove.h"

SpiderIdle::SpiderIdle(Entity* _owner) : EntityState(_owner)
{
}

void SpiderIdle::InitState()
{
	m_duration = (rand() / (float)RAND_MAX) * 0.75f + 0.25f;
	m_owner->SetCurrentAnimation("Idle");
}

void SpiderIdle::Update()
{
	m_timer += GameData::DeltaTime * GameData::GameSpeed;
	m_owner->Move({ 0,0 });
}

void SpiderIdle::ExitState()
{
}

EntityState* SpiderIdle::GetNextState()
{
	if (m_timer > m_duration)
	{
		return new SpiderIdleMove(m_owner);
	}
	return nullptr;
}