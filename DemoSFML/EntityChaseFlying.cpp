#include "EntityStateInclude.h"
#include "common.h"
#include "Entity.h"
#include "CharacterController.h"

EntityChaseFlying::EntityChaseFlying(Entity* entity) : EntityState(entity)
{
}

void EntityChaseFlying::InitState()
{
	m_timer = 10.f;
}

void EntityChaseFlying::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	sf::Vector2f path;
	path = (GameData::CharaCtrl->GetPos() - m_owner->GetScreenPos());
	path /= std::sqrt(DistanceSquared(path, { 0,0 }));
	path *= 25.f;
	m_owner->Move(path * 10.f);

	m_timer -= dt;
}

void EntityChaseFlying::ExitState()
{

}

EntityState* EntityChaseFlying::GetNextState()
{
	float distance = DistanceSquared(m_owner->GetPos(), Screen2World(GameData::CharaCtrl->GetPos()));
	if (m_timer <= 0 || distance >= m_owner->SightRange() * m_owner->SightRange())
	{
		return new EntityIdleFlying(m_owner);
	}
	return nullptr;
}