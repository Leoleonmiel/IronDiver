#include "SpiderIdleMove.h"
#include "common.h"
#include "Entity.h"

#include "SpiderIdle.h"

SpiderIdleMove::SpiderIdleMove(Entity* _owner) : EntityState(_owner)
{
	m_angle = (rand() / (float)RAND_MAX) * 2 * 3.14f;
	m_duration = (rand() / (float)RAND_MAX) * 0.075f + 0.075f;
}

void SpiderIdleMove::InitState()
{
	m_timer = 0.f;
}

void SpiderIdleMove::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	sf::Vector2f move = { std::cos(m_angle), std::sin(m_angle) };
	m_timer += dt;
	move*= m_speed;
	m_owner->Move(move);
}

void SpiderIdleMove::ExitState()
{

}

EntityState* SpiderIdleMove::GetNextState()
{
	if (m_timer >= m_duration)
	{
		return new SpiderIdle(m_owner);
	}
	return nullptr;
}