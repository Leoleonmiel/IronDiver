#include "MantisIdle.h"
#include "MantisJump.h"
#include "MantisEntity.h"

#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "Animator.h"
#include "Box2D_Shape.h"

MantisIdle::MantisIdle(Entity* _owner) : EntityState(_owner) {}

void MantisIdle::InitState()
{
	if (m_owner->GetAnimator()[0]->GetCurrentAnimationName() != "Idle")
	{
		m_owner->SetCurrentAnimation("Idle");
	}
	m_elapsedTime = 0.0f;
	m_owner->GetBody(0)->SetGravityScale(1.f);
	m_owner->GetCoreShape()->SetBounce(0.f);

}

void MantisIdle::Update()
{
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void MantisIdle::ExitState()
{
	m_owner->Move({ 0,0 });
}

EntityState* MantisIdle::GetNextState()
{
	MantisEntity* mantis = reinterpret_cast<MantisEntity*>(m_owner);
	if (m_elapsedTime >= m_timeBeforeJump && mantis->GetOnGround())
	{
		mantis->SetOnGround(false);
		return new MantisJump(m_owner);
	}
	return nullptr;
}
