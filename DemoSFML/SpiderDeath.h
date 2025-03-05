#ifndef SPIDER_DEATH_H
#define SPIDER_DEATH_H

#include "EntityState.h"

class SpiderDeath : public EntityState
{
private:
	const float m_duration = 1.75f;
	float m_timer = 0.f;

	const float m_knockbackStrength = 1200.f;
	float m_knockbackDirection = 0.f;
public:
	SpiderDeath(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
