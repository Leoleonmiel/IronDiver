#ifndef DESTRUCTION_STATE_H
#define DESTRUCTION_STATE_H

#include "EntityState.h"

class DestructionState : public EntityState
{
private:
	float m_timer;
public:
	DestructionState(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !DESTRUCTION_STATE_H
