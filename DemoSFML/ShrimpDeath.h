#ifndef SHRIMP_DEATH_H
#define SHRIMP_DEATH_H

#include "EntityState.h"

class ShrimpDeath : public EntityState
{
private:
	const float m_duration = 1.75f;
	const float m_shrinkDuration = 1.5f;
	float m_timer;

	const float m_knockbackStrength = 1200.f;
	float m_knockbackDirection;
public:
	ShrimpDeath(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
