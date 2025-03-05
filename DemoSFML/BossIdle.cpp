#include "BossIdle.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "BossDash.h"
#include "BossPrepareSlam.h"

BossIdle::BossIdle(Entity* _owner) : EntityState(_owner)
{
}

void BossIdle::InitState()
{
	m_right = GameData::CharaCtrl->GetPos().x < m_owner->GetScreenPos().x;
	m_timer = m_minTime + (rand() / RAND_MAX) * (m_maxTime - m_minTime);
}

void BossIdle::Update()
{
	sf::Vector2f goal = GameData::CharaCtrl->GetPos();
	goal.x += m_right ? 900.f : -900.f;
	sf::Vector2f path = goal - (m_owner->GetScreenPos() - sf::Vector2f(0, 100));
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	float move = dt * m_speed;
	float distance = std::sqrt(DistanceSquared(path, { 0,0 }));

	if (m_owner->GetScreenPos() != goal)
	{
		if (distance <= move)
		{
			m_owner->SetPos(goal);
			m_owner->Move({ 0,0 });
		}
		else
		{
			m_owner->Move(path / distance * m_speed);
		}
	}

	m_timer -= dt;
}

void BossIdle::ExitState()
{

}

EntityState* BossIdle::GetNextState()
{
	if (m_timer <= 0.f && std::abs(m_owner->GetScreenPos().y - GameData::CharaCtrl->GetPos().y) < 100.f)
	{
		if (rand() % 2 == 0)
		{
			return new BossPrepareSlam(m_owner);
		}
		return new BossDash(m_owner);
	}
	return nullptr;
}
