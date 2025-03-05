#ifndef OCTOPUS_BACKGROUND_IDLE_FALL_H
#define OCTOPUS_BACKGROUND_IDLE_FALL_H

#include "EntityState.h"

class OctopusBackgroundIdleFall : public EntityState
{
private:
	const float m_fallSpeed = 50.0f;
	const float m_fallTime = 1.f;
	float m_elapsedTime = 0.f;
public:
	OctopusBackgroundIdleFall(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
