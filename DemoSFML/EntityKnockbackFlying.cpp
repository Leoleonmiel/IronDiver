#include "EntityStateInclude.h"
#include "Entity.h"
#include "common.h"

EntityKnockbackFlying::EntityKnockbackFlying(Entity* _owner) : EntityState(_owner)
{

}

void EntityKnockbackFlying::InitState()
{
	m_timer = 1.5f;
	b2Body* body = m_owner->GetBody(0);
	body->SetGravityScale(0.5f);
	m_owner->SetCurrentAnimation("knockback");
}
void EntityKnockbackFlying::Update()
{
	m_timer -= GameData::DeltaTime * GameData::GameSpeed;
	m_owner->SetPos(m_owner->GetScreenPos());
}
void EntityKnockbackFlying::ExitState()
{
	b2Body* body = m_owner->GetBody(0);
	body->SetGravityScale(0.f);
	m_owner->SetCurrentAnimation("fly");
}
EntityState* EntityKnockbackFlying::GetNextState()
{
	if (m_timer <= 0)
	{
		if (!m_owner->IsCowardly())
		{
			return new EntityRageChaseFlying(m_owner);
		}
		return new EntityIdleFlying(m_owner);
	}
	return nullptr;
}