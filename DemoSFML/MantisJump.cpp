#include "MantisJump.h"
#include "MantisIdle.h"

#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "Animator.h"

#include "Box2D_Shape.h"

MantisJump::MantisJump(Entity* _owner) : EntityState(_owner) {}

void MantisJump::InitState()
{
	if (m_owner->GetAnimator()[0]->GetCurrentAnimationName() != "Jump")
	{
		m_owner->SetCurrentAnimation("Jump");
	}
	m_elapsedTime = 0.0f;
	m_owner->GetCoreShape()->SetBounce(0.f);
	if(m_owner->isLookingRight()) m_owner->Move({m_jumpWidth * m_jumpSpeed * m_jumpTime, -m_jumpHeight * m_jumpSpeed * m_jumpTime });
	else m_owner->Move({-m_jumpWidth * m_jumpSpeed * m_jumpTime, -m_jumpHeight * m_jumpSpeed * m_jumpTime });
}

void MantisJump::Update()
{
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	//Make a jump
}

void MantisJump::ExitState()
{
}

EntityState* MantisJump::GetNextState()
{
	if (m_elapsedTime >= m_jumpTime) 
	{
		return new MantisIdle(m_owner);
	}
	return nullptr;
}
