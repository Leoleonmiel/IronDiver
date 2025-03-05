#ifndef FLY_DEATH_H
#define FLY_DEATH_H

#include "EntityState.h"

class FlyDeath : public EntityState
{
private:
	const float m_deathDuration = 1.4f;
	float m_timer;

	const float m_duration = 1.5f;

	float m_knockbackDirection;
	float m_angle;
	float m_elapsedTime = 0.f;
	bool m_hitboxDead = false;
public:
	FlyDeath(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !FLY_DEATH_H
