#ifndef ENTITY_RAGE_CHASE_FLYING_H
#define ENTITY_RAGE_CHASE_FLYING_H

#include "EntityState.h"

struct Animation;

class EntityRageChaseFlying : public EntityState
{
private:
	float m_timer;
	Animator& m_animator;
public:
	EntityRageChaseFlying(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !ENTITY_CHASE_FLYING_H