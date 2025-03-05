#ifndef SPIDER_IDLE_MOVE_H
#define SPIDER_IDLE_MOVE_H

#include "EntityState.h"

class SpiderIdleMove : public EntityState
{
private:
	float m_angle = 0.f;
	float m_duration = 0.f;
	float m_timer = 0.f;

	const float m_speed = 800.f;
public:
	SpiderIdleMove(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
