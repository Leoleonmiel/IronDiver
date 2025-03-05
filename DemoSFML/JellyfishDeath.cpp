#include "JellyfishDeath.h"
#include "common.h"
#include "EntityManager.h"
#include "Entity.h"

JellyfishDeath::JellyfishDeath(Entity* _owner) : EntityState(_owner)
{

}

void JellyfishDeath::InitState()
{
	m_timer = 0.f;
	m_owner->SetCurrentAnimation("Death");
}

void JellyfishDeath::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	float timeRatio;
	m_timer += dt;

	timeRatio = m_timer / m_duration;

	if (m_timer >= m_duration)
	{
		EntityManager::GetInstance()->MarkForDeath(m_owner);
		return;
	}

	m_angle -= m_rotationSpeed * dt;
	m_owner->SetRotation(m_angle);
	m_owner->Move(sf::Vector2f{ cos(m_angle * 3.14f), sin(m_angle * 3.14f) } * 400.f * (timeRatio * timeRatio));
}

void JellyfishDeath::ExitState()
{

}

EntityState* JellyfishDeath::GetNextState()
{
	return nullptr;
}