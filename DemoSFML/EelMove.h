#ifndef EEL_MOVE_H
#define EEL_MOVE_H

#include "EntityState.h"


class EelMove : public EntityState
{
private:
	const float m_duration = 1.0f;
	float m_timer;

	const float m_speed = 1000.f;
	float desceleringPower = 1.f;
public:
	EelMove(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};


#endif // !EEL_MOVE_H

