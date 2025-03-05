#ifndef FLY_CHASE_H
#define FLY_CHASE_H

#include "EntityState.h"

class FlyChase : public EntityState
{
private:
	const float m_speed = 300.f;
	const float m_attackRange = 15.f;

	bool m_hitboxDead = false;

public:
	FlyChase(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !FLY_CHASE_H