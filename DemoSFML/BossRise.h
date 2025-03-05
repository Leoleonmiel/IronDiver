#ifndef BOSS_RISE_H
#define BOSS_RISE_H

#include "EntityState.h"
#include "SFML/Graphics.hpp"

class BossChaseRise : public EntityState
{
private:
	float m_riseSpeed;
	float m_riseTimer;

public:
	BossChaseRise(Entity* _owner, float _riseSpeed, float _riseTimer);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !BOSS_RISE_H
