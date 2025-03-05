#ifndef ENTITY_KNOCKBACK_FLYING_H
#define ENTITY_KNOCKBACK_FLYING_H

#include "EntityState.h"

class EntityKnockbackFlying : public EntityState
{
private:
	float m_timer;
public:
	EntityKnockbackFlying(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !ENTITY_CHASE_FLYING_H