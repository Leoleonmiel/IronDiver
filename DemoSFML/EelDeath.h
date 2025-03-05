#ifndef EEL_DEATH_H
#define EEL_DEATH_H

#include "EntityState.h"

class EelDeath : public EntityState
{
private:
	const float m_deathDuration = 1.f;
	float m_timer;

	float m_knockbackDirection;
	float m_angle;
	float m_elapsedTime = 0.f;
	bool dying = false;
	bool m_hitboxDead = false;
public:
	EelDeath(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !FLY_DEATH_H
