#include "EntityIdleGeneric.h"
#include <iostream>

EntityIdleGeneric::EntityIdleGeneric(Entity* _owner) : EntityState(_owner)
{
}

void EntityIdleGeneric::InitState()
{
}

void EntityIdleGeneric::Update()
{
}

void EntityIdleGeneric::ExitState()
{
}

EntityState* EntityIdleGeneric::GetNextState()
{
	return nullptr;
}
