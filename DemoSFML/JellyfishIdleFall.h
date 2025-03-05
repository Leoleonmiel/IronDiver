#ifndef JELLYFISH_IDLE_FALL_H
#define JELLYFISH_IDLE_FALL_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class JellyfishIdleFall : public EntityState
{
private:
	const float m_fallSpeed = 0.0f;
	const float m_fallTime = 2.f;
	float m_elapsedTime;
public:
	JellyfishIdleFall(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif