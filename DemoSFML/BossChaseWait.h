#ifndef BOSS_CHASE_WAIT_H
#define BOSS_CHASE_WAIT_H

#include "EntityState.h"

class BossChaseWait : public EntityState
{
private:
	EntityState* m_nextState;
	float m_timer;
	float m_duration;
public:
	BossChaseWait(Entity* _owner, float _duration, EntityState* _nextState);
	void InitState() override;
	void Update() override;
	void ExitState() override;

	EntityState* GetNextState() override;
};

#endif // !
