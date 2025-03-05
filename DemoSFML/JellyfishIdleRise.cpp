#include "JellyfishIdleRise.h"
#include "JellyfishIdleFall.h"

#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "Animator.h"

JellyfishIdleRise::JellyfishIdleRise(Entity* _owner) : EntityState(_owner)
{
}

void JellyfishIdleRise::InitState()
{
	if (m_owner->GetAnimator()[0]->GetCurrentAnimationName() != "Idle")
	{
		m_owner->SetCurrentAnimation("Idle");
	}
	m_elapsedTime = 0.0f;
}

void JellyfishIdleRise::Update()
{
	m_owner->Move(sf::Vector2f{ 0.f, -1.f } *m_riseSpeed);
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void JellyfishIdleRise::ExitState()
{
	m_owner->Move({ 0,0 });
}

EntityState* JellyfishIdleRise::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	if (m_elapsedTime >= m_riseTime)
	{
		return new JellyfishIdleFall(m_owner);
	}
	return nullptr;
}
