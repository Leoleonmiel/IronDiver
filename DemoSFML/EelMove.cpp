#include "EelMove.h"
#include "common.h"
#include "Entity.h"
#include "EelIdle.h"
#include "Box2D_Shape.h"
#include "EelEntity.h"
#include "CharacterController.h"
#include "EelChase.h"

EelMove::EelMove(Entity* _owner) : EntityState(_owner)
{
	m_owner->SetCurrentAnimation("Rush");
}

void EelMove::InitState()
{
	m_timer = 0.f;
}

void EelMove::Update()
{
	EelEntity* eel = static_cast<EelEntity*>(m_owner);
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	desceleringPower -= dt;
	m_timer += dt;
	sf::Vector2f move = { eel->GetDirection(),0};
	move *= (m_speed * desceleringPower);
	m_owner->Move(move);
}

void EelMove::ExitState()
{
}

EntityState* EelMove::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (m_timer >= m_duration)
	{
		return new EelIdle(m_owner);
	}
	else if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange())
	{
		return new EelChase(m_owner);
	}
	return nullptr;
}
