#ifndef BOSS_CHASE_STATE_H
#define BOSS_CHASE_STATE_H

#include "EntityState.h"

class BossChaseState : public EntityState
{
private:
	const float m_horizontalSpeed = 400.f;
	const float m_verticalSpeed = 950.f;
	//const float m_distanceThreshold = 1000.f;
public:
	BossChaseState(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
