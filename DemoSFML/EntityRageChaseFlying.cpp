#include "EntityStateInclude.h"
#include "common.h"
#include "Entity.h"
#include "CharacterController.h"
#include "Animator.h"

EntityRageChaseFlying::EntityRageChaseFlying(Entity* entity) : EntityState(entity), m_animator(*entity->GetAnimator()[0])
{
}

void EntityRageChaseFlying::InitState()
{
	m_timer = 10.f;
	m_animator.SetAnimatorSpeed(2.f);
	m_animator.TrySetCurrentAnimation("fly");
}

void EntityRageChaseFlying::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;
	sf::Vector2f path;
	path = (GameData::CharaCtrl->GetPos() - m_owner->GetScreenPos());
	path /= std::sqrt(DistanceSquared(path, { 0,0 }));
	path *= dt * 45.f;
	m_owner->Move(path * 10.f);

	m_timer -= dt;
}

void EntityRageChaseFlying::ExitState()
{
	m_animator.SetAnimatorSpeed(1.0f);
}

EntityState* EntityRageChaseFlying::GetNextState()
{
	float distance = DistanceSquared(m_owner->GetPos(), Screen2World(GameData::CharaCtrl->GetPos()));
	if (m_timer <= 0)
	{
		return new EntityIdleFlying(m_owner);
	}
	return nullptr;
}