#include "FlyAttack.h"

#include <queue>
#include "Entity.h"
#include "Animator.h"
#include "common.h"
#include "CharacterController.h"
#include "FlyChase.h"
#include "FlyIdle.h"

FlyAttack::FlyAttack(Entity* _owner) : EntityState(_owner) {}

void FlyAttack::InitState()
{
	m_owner->SetCurrentAnimation("Attack"); 
	m_elapsedTime = 0.f;
	m_owner->GetBody(1)->SetEnabled(true);
}

void FlyAttack::Update()
{
	m_elapsedTime += GameData::DeltaTime;	
}

void FlyAttack::ExitState()
{
	
}

EntityState* FlyAttack::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (m_elapsedTime > m_delay) 
	{
		if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange())
		{
			return new FlyChase(m_owner);
		}
		else
		{
			return new FlyIdle(m_owner);
		}
	}
	return nullptr;
}
