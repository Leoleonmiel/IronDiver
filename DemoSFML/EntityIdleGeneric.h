#pragma once
#include "EntityState.h"

class EntityIdleGeneric :
    public EntityState
{
private:

public:
	EntityIdleGeneric(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

