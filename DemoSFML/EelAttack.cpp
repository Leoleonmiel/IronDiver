#include "EelAttack.h"
#include <queue>
#include "Entity.h"
#include "Animator.h"
#include "common.h"
#include "CharacterController.h"
#include "EelIdle.h"
#include "EelChase.h"

EelAttack::EelAttack(Entity* _owner) : EntityState(_owner) {}

void EelAttack::InitState()
{
	m_elapsedTime = 0.f;
	m_owner->GetBody(1)->SetEnabled(true);
}

void EelAttack::Update()
{
	m_elapsedTime += GameData::DeltaTime;
}

void EelAttack::ExitState()
{
	
}

EntityState* EelAttack::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (m_elapsedTime > m_duration)
	{
		if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange())
		{
			return new EelChase(m_owner);
		}
		else
		{
			return new EelIdle(m_owner);
		}
	}
	return nullptr;
}
