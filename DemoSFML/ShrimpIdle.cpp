#include "ShrimpIdle.h"
#include "Entity.h"

ShrimpIdle::ShrimpIdle(Entity* _owner) : EntityState(_owner)
{
}

void ShrimpIdle::InitState()
{
	m_owner->SetCurrentAnimation("Idle");
	m_owner->Move({ 0,0 });
}

void ShrimpIdle::Update()
{
}

void ShrimpIdle::ExitState()
{
}

EntityState* ShrimpIdle::GetNextState()
{
	return nullptr;
}
