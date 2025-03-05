#include "OctopusIdleFall.h"
#include "OctopusIdleRise.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "OctopusChase.h"



OctopusIdleFall::OctopusIdleFall(Entity* _owner) : EntityState(_owner)
{

}

void OctopusIdleFall::InitState()
{
	m_owner->SetCurrentAnimation("FreezeAnim");
	m_elapsedTime = 0.f;
}

void OctopusIdleFall::Update()
{
	m_owner->Move(sf::Vector2f{ 0.f, 1.f } *m_fallSpeed);
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void OctopusIdleFall::ExitState()
{
	m_owner->Move({ 0,0 });
}

EntityState* OctopusIdleFall::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (m_elapsedTime >= m_fallTime)
	{
		return new OctopusIdleRise(m_owner);
	}
	else if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange())
	{
		return new OctopusChase(m_owner);
	}
	return nullptr;
}