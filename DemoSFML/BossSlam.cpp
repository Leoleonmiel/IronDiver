#include "BossSlam.h"
#include "common.h"
#include "Entity.h"
#include "BossIdle.h"
#include "BossShockwave.h"
#include "BossShockwaveState.h"
#include "Boss.h"
#include "EntityManager.h"
#include "Animator.h"
#include "CameraManager.h"
#include <queue>

BossSlam::BossSlam(Entity* _owner) : EntityState(_owner)
{
}

void BossSlam::InitState()
{
	m_timer = m_duration;
	m_owner->SetCurrentAnimation("PrepareSlam");
}

void BossSlam::Update()
{
	/*float dt = GameData::DeltaTime * GameData::GameSpeed;
	m_timer -= dt;*/

	if (m_owner->GetScreenPos().y < Boss::ShockwaveSpawnPoint.y - 100.f)
	{
		m_owner->Move({ 0, m_speed });
	}
}

void BossSlam::ExitState()
{
	if (m_owner->GetScreenPos().y >= Boss::ShockwaveSpawnPoint.y - 100.f)
	{
		std::queue<Animation*> animations;
		animations.push(m_owner->GetAnimation("Onde"));
		animations.push(m_owner->GetAnimation("Idle"));

		m_owner->GetAnimator()[0]->SetAnimationQueue(animations);

		Entity* shockwave = nullptr;
		EntityManager& manager = *EntityManager::GetInstance();
		shockwave = new BossShockwave(true, Boss::ShockwaveSpawnPoint + sf::Vector2f(0, -75));
		shockwave->SetState(new BossShockwaveState(shockwave, true));
		manager.Add(shockwave);

		shockwave = new BossShockwave(false, Boss::ShockwaveSpawnPoint + sf::Vector2f(0, -75));
		shockwave->SetState(new BossShockwaveState(shockwave, false));
		manager.Add(shockwave);

		CameraManager::PrepareForShake(0.5f, 0.2f, 25.f);
		CameraManager::Shake(0.5f);
	}
}

EntityState* BossSlam::GetNextState()
{
	if (m_owner->GetScreenPos().y >= Boss::ShockwaveSpawnPoint.y - 100.f)
	{
		return new BossIdle(m_owner);
	}
	return nullptr;
}
