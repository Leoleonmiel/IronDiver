#include "MantisIdleRise.h"
#include "MantisIdleFall.h"

#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "Animator.h"

MantisIdleRise::MantisIdleRise(Entity* _owner) : EntityState(_owner)
{
}

void MantisIdleRise::InitState()
{
	if (m_owner->GetAnimator()[0]->GetCurrentAnimationName() != "Idle")
	{
		m_owner->SetCurrentAnimation("Idle");
	}
	m_elapsedTime = 0.0f;
}

void MantisIdleRise::Update()
{
	m_owner->Move(sf::Vector2f{ 0.f, -1.f } *m_riseSpeed);
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void MantisIdleRise::ExitState()
{
	m_owner->Move({ 0,0 });
}

EntityState* MantisIdleRise::GetNextState()
{
	return nullptr;
}
