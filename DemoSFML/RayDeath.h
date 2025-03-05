#ifndef RAY_DEATH_H
#define RAY_DEATH_H

#include "EntityState.h"

class RayDeath : public EntityState
{
private:
	const float m_duration = 0.75f;
	const float m_shrinkDuration = 1.5f;

	const float m_knockbackStrength = 1200.f;

	float m_timer = 0.f;
public:
	RayDeath(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !RAY_DEATH_H
