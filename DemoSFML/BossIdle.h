#ifndef BOSS_IDLE_H
#define BOSS_IDLE_H

#include "EntityState.h"

class BossIdle : public EntityState
{
private:
	bool m_right;
	const float m_speed = 250.f;

	float m_timer;
	const float m_minTime = 1.5f;
	const float m_maxTime = 3.5f;
public:
	BossIdle(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif