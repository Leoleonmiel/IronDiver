#ifndef BOSS_CHASE_END_H
#define BOSS_CHASE_END_H

#include "EntityState.h"

class BossChaseEnd : public EntityState
{
private:
	const float m_duration = 1.5f;
	const float m_speed = 1250.f;

	float m_timer;
public:
	BossChaseEnd(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
