#include "OctopusAttack.h"
#include <queue>
#include "Entity.h"
#include "Animator.h"
#include "common.h"
#include "CharacterController.h"
#include "OctopusChase.h"

OctopusAttack::OctopusAttack(Entity* _owner) : EntityState(_owner)
{
}

void OctopusAttack::InitState()
{
	m_owner->SetCurrentAnimation("AttackLoop");
	//std::queue<Animation*> animations;

	//animations.push(m_owner->GetAnimation("AttackLoop"));

	//m_owner->GetAnimator()[0]->SetAnimationQueue(animations);

	m_elapsedTime = 0.f;
	m_owner->GetBody(1)->SetEnabled(true);
}

void OctopusAttack::Update()
{
	sf::Vector2f path = m_owner->GetScreenPos() - (GameData::CharaCtrl->GetPos() - sf::Vector2f{ 0, ZOOM * 10.f });
	if (path.x > 0)
	{
		m_owner->SetRotation(15.f);
	}
	else
	{
		m_owner->SetRotation(-15.f);
	}

	m_owner->Move({ 0,0 });

	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void OctopusAttack::ExitState()
{
	m_owner->SetRotation(0);
	m_owner->GetBody(1)->SetEnabled(false);
}

EntityState* OctopusAttack::GetNextState()
{
	if (m_elapsedTime >= m_duration)
	{
		return new OctopusChase(m_owner);
	}
	return nullptr;
}