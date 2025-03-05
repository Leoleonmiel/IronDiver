#include "BossEntry.h"
#include "Entity.h"
#include "Boss.h"
#include "BossIdle.h"
#include "BossChaseWait.h"

BossEntry::BossEntry(Entity* _owner) : EntityState(_owner)
{
}

void BossEntry::InitState()
{

}

void BossEntry::Update()
{
	m_owner->Move({ 0, m_speed });
}

void BossEntry::ExitState()
{

}

EntityState* BossEntry::GetNextState()
{
	if (m_owner->GetScreenPos().y >= Boss::BeginPoint.y)
	{
		return new BossChaseWait(m_owner, 1.0f, new BossIdle(m_owner));
	}

	return nullptr;
}
