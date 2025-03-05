#include "EntityStateInclude.h"
#include "Entity.h"
#include "CharacterController.h"
#include "common.h"

EntityRunawayFlying::EntityRunawayFlying(Entity* _owner) : EntityState(_owner)
{
}

void EntityRunawayFlying::InitState()
{
	m_owner->SetCurrentAnimation("fly");
}

void EntityRunawayFlying::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	sf::Vector2f path;
	path = (m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos());
	path /= std::sqrt(DistanceSquared(path, { 0,0 }));
	path *= 25.f;
	m_owner->Move(path * 10.f);
}

void EntityRunawayFlying::ExitState()
{
}

EntityState* EntityRunawayFlying::GetNextState()
{
	float distance = DistanceSquared(m_owner->GetPos(), Screen2World(GameData::CharaCtrl->GetPos()));
	if (distance > (1.5f * m_owner->SightRange()) * (1.5f * m_owner->SightRange()))
	{
		return new EntityIdleFlying(m_owner);
	}
	return nullptr;
}
