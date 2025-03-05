#include "BossRise.h"
#include "BossChaseState.h"
#include "BossChaseWait.h"
#include "BossChase.h"
#include "Entity.h"
#include "common.h"

BossChaseRise::BossChaseRise(Entity* _owner, float _riseSpeed, float _riseTimer) : EntityState(_owner)
{
	m_riseSpeed = _riseSpeed;
	m_riseTimer = _riseTimer;
}

void BossChaseRise::InitState()
{
	m_owner->SetCurrentAnimation("Move");
}

void BossChaseRise::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	float move = dt * m_riseSpeed;
	float path = BossChase::CinematicCameraBegin.y - m_owner->GetScreenPos().y;

	if (std::abs(path) <= move)
	{
		m_owner->SetPos({ m_owner->GetScreenPos().x, BossChase::CinematicCameraBegin.y });
	}
	else
	{
		m_owner->Move({ 0, m_riseSpeed });
	}

	m_riseTimer -= dt;

	
	m_owner->Move({ 0, -m_riseSpeed });
}

void BossChaseRise::ExitState()
{
	return;
}

EntityState* BossChaseRise::GetNextState()
{	
	float path = BossChase::CinematicCameraBegin.y - m_owner->GetScreenPos().y;
	if (path == 0.f)
	{
		m_owner->Move({ 0,0 });
		return new BossChaseWait(m_owner, -1.f, nullptr);
	}
	return nullptr;
}
