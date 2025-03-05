#include "FlyDeath.h"
#include "Entity.h"
#include "Animator.h"
#include "EntityManager.h"
#include "Animation.h"

FlyDeath::FlyDeath(Entity* _owner) : EntityState(_owner) {}

void FlyDeath::InitState()
{
	m_owner->GetAnimator()[1]->TrySetCurrentAnimation("None");
	m_owner->SetCurrentAnimation("Death");
	m_owner->GetBody(0)->SetGravityScale(2.f);
	m_owner->GetBody(1)->SetAwake(true);
}

void FlyDeath::Update()
{
	if (!m_hitboxDead)
	{
		m_owner->GetBody(1)->SetEnabled(false);
		m_hitboxDead = true;
	}

	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
	if (m_owner->GetAnimator()[0]->LoopedOnce() && m_elapsedTime > m_duration)
	{
		EntityManager::GetInstance()->MarkForDeath(m_owner);
	}
}

void FlyDeath::ExitState()
{
}

EntityState* FlyDeath::GetNextState()
{
	return nullptr;
}
