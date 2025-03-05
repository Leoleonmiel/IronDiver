#include "EelDeath.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Animation.h"
#include "Box2D_Shape.h"
#include "Animator.h"
#include "Box2D_Shape.h"
#include "EelEntity.h"

EelDeath::EelDeath(Entity* _owner) : EntityState(_owner)
{
}

void EelDeath::InitState()
{
	m_owner->SetCurrentAnimation("WaitingDeath");
	m_owner->FreezeBodies();
	m_owner->GetBody(0)->SetGravityScale(1.f);
	EelEntity* eel = static_cast<EelEntity*>(m_owner);
	eel->SetIsCollided(false);
	eel->SetAngle(0);
}

void EelDeath::Update()
{
	if (!m_hitboxDead)
	{
		m_owner->GetBody(1)->SetEnabled(false);
		m_hitboxDead = true;
	}
	EelEntity* eel = static_cast<EelEntity*>(m_owner);
	m_timer += GameData::DeltaTime * GameData::GameSpeed;
	if (dying && m_owner->GetAnimator()[0]->LoopedOnce())
	{
		EntityManager::GetInstance()->MarkForDeath(m_owner);
	}
	if (eel->IsCollided() || m_timer > m_deathDuration)
	{
		if (!dying)
		{
			m_owner->SetCurrentAnimation("Death");
			dying = true;
		}
	}
}

void EelDeath::ExitState()
{
}

EntityState* EelDeath::GetNextState()
{
	return nullptr;
}
