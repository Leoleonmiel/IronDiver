#ifndef CREATE_STATE_H
#define CREATE_STATE_H

#include "EntityState.h"



class CreateState : public EntityState
{
private:
	float m_timer;

public:
	CreateState(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !CREATE_STATE_H
