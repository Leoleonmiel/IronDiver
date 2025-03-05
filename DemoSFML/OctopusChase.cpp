#include "OctopusChase.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "OctopusAttack.h"



OctopusChase::OctopusChase(Entity* _owner) : EntityState(_owner)
{
}

void OctopusChase::InitState()
{
	m_owner->SetCurrentAnimation("BeginAttack");
}

void OctopusChase::Update()
{
	sf::Vector2f path = m_owner->GetScreenPos() - (GameData::CharaCtrl->GetPos() - sf::Vector2f{0, ZOOM * 10.f});
	if (path.x > 0)
	{
		m_owner->SetRotation(15.f);
	}
	else
	{
		m_owner->SetRotation(-15.f);
	}
	
	path *= -1.f;
	path /= std::sqrt(DistanceSquared(path, { 0,0 }));
	path /= ZOOM;

	path *= m_speed * ZOOM;
	m_owner->Move(path);
}

void OctopusChase::ExitState()
{
	m_owner->SetRotation(0);
}

EntityState* OctopusChase::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (DistanceSquared(path, { 0,0 }) <= 100.f)
	{
		return new OctopusAttack(m_owner);
	}
	return nullptr;
}