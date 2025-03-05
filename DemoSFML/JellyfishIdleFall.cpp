#include "JellyfishIdleFall.h"
#include "JellyfishIdleRise.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "Animator.h"

JellyfishIdleFall::JellyfishIdleFall(Entity* _owner) : EntityState(_owner)
{
}

void JellyfishIdleFall::InitState()
{
	if (m_owner->GetAnimator()[0]->GetCurrentAnimationName() != "Idle")
	{
		m_owner->SetCurrentAnimation("Idle");
	}
	m_elapsedTime = 0.f;
}

void JellyfishIdleFall::Update()
{
	m_owner->Move(sf::Vector2f{ 0.f, 1.f } *m_fallSpeed);
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void JellyfishIdleFall::ExitState()
{
	m_owner->Move({ 0,0 });
}

EntityState* JellyfishIdleFall::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (m_elapsedTime >= m_fallTime)
	{
		return new JellyfishIdleRise(m_owner);
	}

	return nullptr;
}