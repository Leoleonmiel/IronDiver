#pragma once
#include "EntityState.h"

class Animator;

class EntityIdleFlying : public EntityState
{
private:
	Animator& m_animator;

public:
	EntityIdleFlying(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

