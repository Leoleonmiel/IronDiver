#include "BossDash.h"
#include "common.h"
#include "Entity.h"
#include "CharacterController.h"
#include "BossIdle.h"
#include "Boss.h"

BossDash::BossDash(Entity* _owner) : EntityState(_owner)
{
}

void BossDash::InitState()
{
	m_timer = m_duration + m_prepareDuration;
	m_right = m_owner->GetScreenPos().x < GameData::CharaCtrl->GetPos().x;
	m_owner->SetCurrentAnimation("PrepareDash");
}

void BossDash::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	m_timer -= dt;

	if (!m_dashing)
	{
		if (m_timer > m_duration)
		{
			m_owner->Move({ m_right ? -m_speed : m_speed, 0 });
		}
		else
		{
			m_dashing = true;
			m_owner->SetCurrentAnimation("Dash");
			m_owner->Move({ m_right ? m_speed : -m_speed, 0 });
			Boss::BossAttacking = true;
			m_owner->SetHitboxEnabled(0, true);
		}
		return;
	}

	m_owner->Move({ m_right ? m_speed : -m_speed, 0 });
}

void BossDash::ExitState()
{
	m_owner->Move({ 0,0 });
	m_owner->SetHitboxEnabled(0, false);
	Boss::BossAttacking = false;

	m_owner->SetCurrentAnimation("Idle");
}

EntityState* BossDash::GetNextState()
{
	if (m_timer <= 0)
	{
		return new BossIdle(m_owner);
	}
	return nullptr;
}