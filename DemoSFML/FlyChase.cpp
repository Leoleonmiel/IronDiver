#include "FlyChase.h"

#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "FlyAttack.h"
#include "FlyIdle.h"


FlyChase::FlyChase(Entity* _owner) : EntityState(_owner) {}

void FlyChase::InitState()
{
	m_owner->SetCurrentAnimation("BeginAttack");
	m_owner->GetBody(1)->SetEnabled(false);
}

void FlyChase::Update()
{
	if (!m_hitboxDead)
	{
		m_owner->GetBody(1)->SetEnabled(false);
		m_hitboxDead = true;
	}
	sf::Vector2f path = m_owner->GetScreenPos() - (GameData::CharaCtrl->GetPos() - sf::Vector2f{ 0, 0 });
	path *= -1.f;
	path /= std::sqrt(DistanceSquared(path, { 0,0 }));
	path /= ZOOM;

	path *= m_speed * ZOOM;
	m_owner->Move(path);
}

void FlyChase::ExitState()
{
}

EntityState* FlyChase::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (sqrt(DistanceSquared(path, { 0,0 })) <= m_attackRange)
	{
		return new FlyAttack(m_owner);
	}
	else if (DistanceSquared(path, { 0,0 }) > m_owner->SightRange() * m_owner->SightRange()) 
	{
		return new FlyIdle(m_owner);
	}
	return nullptr;
}
