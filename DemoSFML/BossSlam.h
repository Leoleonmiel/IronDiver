#ifndef BOSS_SLAM_H
#define BOSS_SLAM_H

#include "EntityState.h"

class BossSlam : public EntityState
{
private:
	const float m_speed = 875.f;
	const float m_duration = 1.f;
	float m_timer = 0.f;
public:
	BossSlam(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
