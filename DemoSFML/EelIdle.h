#ifndef EEL_IDLE_H
#define EEL_IDLE_H

#include "EntityState.h"

class EelIdle : public EntityState
{
protected:
	const float m_duration = 1.f;
	float m_elapsedTime = 0.f;
	float m_minCapX = -20.f;
	float m_maxCapX = 20.f;
	bool m_hitboxDead = false;
public:
	EelIdle(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !

