#ifndef JELLYFISH_DEATH_H
#define JELLYFISH_DEATH_H

#include "EntityState.h"

class JellyfishDeath : public EntityState
{
private:
	const float m_duration = 1.6f;
	float m_timer;

	const float m_rotationSpeed = 1.3f;
	float m_angle;

public:
	JellyfishDeath(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !JELLYFISH_DEATH_H
