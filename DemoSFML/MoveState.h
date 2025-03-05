#ifndef MOVE_STATE_H
#define MOVE_STATE_H

#include "EntityState.h"

class MoveState : public EntityState
{
private:
	float m_timer;
public:
	MoveState(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !MOVE_STATE_H
