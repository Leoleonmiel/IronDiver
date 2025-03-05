#include "OctopusDeath.h"
#include "Entity.h"
#include "Animator.h"
#include "EntityManager.h"

OctopusDeath::OctopusDeath(Entity* _owner) : EntityState(_owner)
{
}

void OctopusDeath::InitState()
{
	m_owner->SetCurrentAnimation("Death");
	m_owner->GetSprite()->setOrigin({ 500, 500 });
	m_owner->GetSprite()->setScale({ 0.5f, 0.5f });
	m_animator = m_owner->GetAnimator()[0];
}

void OctopusDeath::Update()
{
	if (m_animator->LoopedOnce())
	{
		EntityManager::GetInstance()->MarkForDeath(m_owner);
	}
}

void OctopusDeath::ExitState()
{
}

EntityState* OctopusDeath::GetNextState()
{
	return nullptr;
}
