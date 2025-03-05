#ifndef SHRIMP_IDLE_H
#define SHRIMP_IDLE_H

#include "EntityState.h"

class ShrimpIdle : public EntityState
{
public:
	ShrimpIdle(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
