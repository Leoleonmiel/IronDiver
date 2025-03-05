#include "OctopusBackgroundIdleFall.h"
#include "OctopusBackgroundIdleRise.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"


OctopusBackgroundIdleFall::OctopusBackgroundIdleFall(Entity* _owner) : EntityState(_owner)
{
}

void OctopusBackgroundIdleFall::InitState()
{
	m_owner->SetCurrentAnimation("FreezeAnim");
	m_elapsedTime = 0.f;
}

void OctopusBackgroundIdleFall::Update()
{
	m_owner->Move(sf::Vector2f{ 0.f, 1.f } *m_fallSpeed);
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void OctopusBackgroundIdleFall::ExitState()
{
	m_owner->Move({ 0,0 });

}

EntityState* OctopusBackgroundIdleFall::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (m_elapsedTime >= m_fallTime)
	{
		return new OctopusBackgroundIdleRise(m_owner);
	}
	return nullptr;
}


