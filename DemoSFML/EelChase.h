#ifndef EEL_CHASE_H
#define EEL_CHASE_H

#include "EntityState.h"

class EelChase : public EntityState 
{
protected:
	const float m_speed = 500.f;
	const float m_attackRange = 10.f;
	const float m_duration = 1.5f;
	float m_timer;
	bool m_hitboxDead = false;
public:
	EelChase(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;

};

#endif // !

