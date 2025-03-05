#include "RayIdle.h"
#include "Entity.h"
#include "Animator.h"
#include "common.h"

RayIdle::RayIdle(Entity* _owner, bool _right, float _duration) : EntityState(_owner)
{
	m_goingRight = _right;
	m_duration = _duration;

}

void RayIdle::InitState()
{
	m_timer = 0.f;
	if (m_owner->GetAnimator()[0]->GetCurrentAnimationName() != "Idle")
	{
		m_owner->SetCurrentAnimation("Idle");
	}
}

void RayIdle::Update()
{
	m_timer += GameData::DeltaTime * GameData::GameSpeed;

	if (m_goingRight)
	{
		m_owner->Move({ m_speed,0 });
	}
	else
	{
		m_owner->Move({ -m_speed,0 });
	}
	
}

void RayIdle::ExitState()
{

}

EntityState* RayIdle::GetNextState()
{
	if (m_timer >= m_duration)
	{
		return new RayIdle(m_owner, !m_goingRight, m_duration);
	}
	return nullptr;
}