#ifndef ENTITY_CHASE_FLYING_H
#define ENTITY_CHASE_FLYING_H

#include "EntityState.h"

class EntityChaseFlying : public EntityState
{
private:
	float m_timer;
public:
	EntityChaseFlying(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !ENTITY_CHASE_FLYING_H
