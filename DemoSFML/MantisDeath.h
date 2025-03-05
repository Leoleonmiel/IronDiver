#ifndef MANTIS_DEATH_H
#define MANTIS_DEATH_H

#include "EntityState.h"

class MantisDeath : public EntityState
{
private:
	const float m_deathDuration = 1.4f;
	float m_timer = 0.f;

	const float m_duration = 1.5f;
	const float m_baseKnockback = 1250.0f;

	float m_knockbackDirection = 0.f;
	float m_angle = 0.f;
	float m_elapsedTime = 0.f;

public:
	MantisDeath(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !MANTIS_DEATH_H
