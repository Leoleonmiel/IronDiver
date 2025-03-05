#ifndef MANTIS_IDLE_H
#define MANTIS_IDLE_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class MantisIdle : public EntityState
{
private:
	const float m_timeBeforeJump = 0.5f;

	float m_elapsedTime = 0.f;
public:
	MantisIdle(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !

