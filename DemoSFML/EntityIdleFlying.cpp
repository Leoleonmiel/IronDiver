
#include "EntityStateInclude.h"
#include "Entity.h"
#include "CharacterController.h"
#include "Animation.h"
#include "common.h"
#include "Animator.h"

EntityIdleFlying::EntityIdleFlying(Entity* _owner) : EntityState(_owner), m_animator(*_owner->GetAnimator()[0])
{
}

void EntityIdleFlying::InitState()
{
	m_owner->FreezeBodies();
	//m_owner->SetCurrentAnimation("fly");

	m_animator.SetAnimatorSpeed(0.75f);
	
}

void EntityIdleFlying::Update()
{
	m_owner->Move({ 0.f,0.f });
}

void EntityIdleFlying::ExitState()
{
	m_animator.SetAnimatorSpeed(1.0f);
}

EntityState* EntityIdleFlying::GetNextState()
{
	if (DistanceSquared(m_owner->GetPos(), Screen2World(GameData::CharaCtrl->GetPos())) <= (m_owner->SightRange() * m_owner->SightRange()))
	{
		if (m_owner->IsCowardly())
		{
			return new EntityRunawayFlying(m_owner);
		}
		else
		{
			return new EntityChaseFlying(m_owner);
		}
	}

	return nullptr;
}
