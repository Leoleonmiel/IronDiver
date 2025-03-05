#ifndef MANTIS_IDLE_RISE_H
#define MANTIS_IDLE_RISE_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class MantisIdleRise : public EntityState
{
private:
	const float m_riseSpeed = 0.0f;
	const float m_riseTime = 2.f;
	float m_elapsedTime = 0.f;
public:
	MantisIdleRise(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !

