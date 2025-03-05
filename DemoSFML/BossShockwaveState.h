#ifndef BOSS_SHOCKWAVE_STATE_H
#define BOSS_SHOCKWAVE_STATE_H

#include "EntityState.h"


class BossShockwaveState : public EntityState
{
private:
	const float m_speed = 750.f;
	const float m_lifetime = 3.5f;
	float m_timer;
	bool m_right;
public:
	BossShockwaveState(Entity* _owner, bool _right);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
