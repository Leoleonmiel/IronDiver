#ifndef BOSS_PREPARE_SLAM_H
#define BOSS_PREPARE_SLAM_H

#include "EntityState.h"

class BossPrepareSlam : public EntityState
{
private:
	const float m_speed = 850.f;
public:
	BossPrepareSlam(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
