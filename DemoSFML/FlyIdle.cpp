#include "FlyIdle.h"
#include "FlyChase.h"

#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "Animator.h"

FlyIdle::FlyIdle(Entity* _owner) : EntityState(_owner) {}

void FlyIdle::InitState()
{

	m_owner->SetCurrentAnimation("Idle");
	m_owner->Move({ 0,0 });
	m_owner->GetBody(1)->SetEnabled(false);

	m_elapsedTime = 0.0f;
}

void FlyIdle::Update()
{
	if (!m_hitboxDead)
	{
		m_owner->GetBody(1)->SetEnabled(false);
		m_hitboxDead = true;
	}
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;

	
}

void FlyIdle::ExitState()
{
}

EntityState* FlyIdle::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange()) 
	{
		return new FlyChase(m_owner);
	}
	return nullptr;
}
