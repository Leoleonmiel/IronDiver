#include "BossChaseState.h"
#include "Entity.h"
#include "CharacterController.h"
#include "BossChaseEnd.h"
#include "BossChaseWait.h"
#include "common.h"
#include "BossChase.h"

BossChaseState::BossChaseState(Entity* _owner) : EntityState(_owner)
{
	
}

void BossChaseState::InitState()
{
	m_owner->SetCurrentAnimation("Move");
}

void BossChaseState::Update()
{
	sf::Vector2f pos = m_owner->GetScreenPos();
	sf::Vector2f path = (GameData::CharaCtrl->GetPos() - pos);
	sf::Vector2f move;
	float dt = GameData::DeltaTime * GameData::GameSpeed;

	if (std::abs(path.y) < m_verticalSpeed * dt)
	{
		m_owner->SetPos({m_owner->GetScreenPos().x, GameData::CharaCtrl->GetPos().y });
		move.y = 0;
	}
	else if (path.y > 0.f)
	{
		move.y = +m_verticalSpeed;
	}
	else
	{
		move.y = -m_verticalSpeed;
	}

	if (path.x < 1000.f)
	{
		move.x = m_horizontalSpeed;
	}
	else
	{
		float value = ((int)(path.x / 1000.f));
		value++;

		move.x = (value + 0.5f) * m_horizontalSpeed;
	}
	

	m_owner->Move(move);
}

void BossChaseState::ExitState()
{

}

EntityState* BossChaseState::GetNextState()
{
	if (m_owner->GetScreenPos().x >= BossChase::EndPoint.x)
	{
		BossChase::BossReachedEnd = true;
		return new BossChaseWait(m_owner, -1.f , nullptr);
	}

	return nullptr;
}