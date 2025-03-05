#include "OctopusBackgroundIdleRise.h"
#include "OctopusBackgroundIdleFall.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"

OctopusBackgroundIdleRise::OctopusBackgroundIdleRise(Entity* _owner) : EntityState(_owner)
{

}

void OctopusBackgroundIdleRise::InitState()
{
	m_owner->SetCurrentAnimation("RiseAnim");
	m_elapsedTime = 0.0f;
}

void OctopusBackgroundIdleRise::Update()
{
	m_owner->Move(sf::Vector2f{ 0.f, -1.f } *m_riseSpeed);
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void OctopusBackgroundIdleRise::ExitState()
{
	m_owner->Move({ 0,0 });
}

EntityState* OctopusBackgroundIdleRise::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	if (m_elapsedTime >= m_riseTime)
	{
		return new OctopusBackgroundIdleFall(m_owner);
	}
	return nullptr;
}
