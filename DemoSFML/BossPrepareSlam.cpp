#include "BossPrepareSlam.h"
#include "Boss.h"
#include "SFML/Graphics.hpp"
#include "common.h"
#include "BossSlam.h"

BossPrepareSlam::BossPrepareSlam(Entity* _owner) : EntityState(_owner)
{
}

void BossPrepareSlam::InitState()
{

}

void BossPrepareSlam::Update()
{
	sf::Vector2f path = (Boss::BossSpawnPoint - m_owner->GetScreenPos());
	float distance = std::sqrt(DistanceSquared(path, { 0,0 }));
	float dt = GameData::DeltaTime * GameData::GameSpeed;

	if (distance <= dt * m_speed)
	{
		m_owner->SetPos(Boss::BossSpawnPoint);
		m_owner->Move({ 0,0 });
	}
	else
	{
		path /= distance;
		path *= m_speed;
		m_owner->Move(path);
	}
}

void BossPrepareSlam::ExitState()
{

}

EntityState* BossPrepareSlam::GetNextState()
{
	if (m_owner->GetScreenPos() == Boss::BossSpawnPoint)
	{
		return new BossSlam(m_owner);
	}
	return nullptr;
}
