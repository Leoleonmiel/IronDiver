#include "OctopusIdleRise.h"
#include "OctopusIdleFall.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "OctopusChase.h"

OctopusIdleRise::OctopusIdleRise(Entity* _owner) : EntityState(_owner)
{
	
}

void OctopusIdleRise::InitState()
{
	m_owner->SetCurrentAnimation("RiseAnim");
	m_elapsedTime = 0.0f;
}

void OctopusIdleRise::Update()
{
	m_owner->Move(sf::Vector2f{ 0.f, -1.f } * m_riseSpeed);
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void OctopusIdleRise::ExitState()
{
	m_owner->Move({ 0,0 });
}

EntityState* OctopusIdleRise::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	if (m_elapsedTime >= m_riseTime)
	{
		return new OctopusIdleFall(m_owner);
	}
	else if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange())
	{
		return new OctopusChase(m_owner);
	}
	return nullptr;
}
