#ifndef BOSS_DASH_H
#define BOSS_DASH_H

#include "EntityState.h"

class BossDash : public EntityState
{
private:
	const float m_speed = 1000.f;
	const float m_duration = 2.0f;
	const float m_prepareDuration = 0.5f;
	float m_timer;

	bool m_right;
	bool m_dashing = false;
public:
	BossDash(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
