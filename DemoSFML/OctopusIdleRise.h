#ifndef OCTOPUS_IDLE_RISE_H
#define OCTOPUS_IDLE_RISE_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class OctopusIdleRise : public EntityState
{
private:
	const float m_riseSpeed = 200.0f;
	const float m_riseTime = 0.5f;
	float m_elapsedTime = 0.f;
public:
	OctopusIdleRise(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !OCTOPUS_IDLE_H
