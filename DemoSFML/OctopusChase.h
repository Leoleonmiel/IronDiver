#ifndef OCTOPUS_CHASE_H
#define OCTOPUS_CHASE_H

#include "EntityState.h"

class OctopusChase : public EntityState
{
private:
	const float m_speed = 150.f;
public:
	OctopusChase(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !OCTOPUS_CHASE_H