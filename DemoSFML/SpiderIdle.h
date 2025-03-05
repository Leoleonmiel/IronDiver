#ifndef SPIDER_IDLE_H
#define SPIDER_IDLE_H

#include "EntityState.h"

class SpiderIdle : public EntityState
{
private:
	float m_duration = 0.f;
	float m_timer = 0.f;
public:
	SpiderIdle(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif