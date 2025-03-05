#ifndef OCTOPUS_DEATH_H
#define OCTOPUS_DEATH_H

#include "EntityState.h"

class Animator;

class OctopusDeath : public EntityState
{
private:
	Animator* m_animator = nullptr;
public:
	OctopusDeath(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !OCTOPUS_DEATH_H
