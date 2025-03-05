#ifndef OCTOPUS_IDLE_FALL_H
#define OCTOPUS_IDLE_FALL_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class OctopusIdleFall : public EntityState
{
private:
	const float m_fallSpeed = 100.0f;
	const float m_fallTime = 1.f;
	float m_elapsedTime = 0.f;
public:
	OctopusIdleFall(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif