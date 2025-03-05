#ifndef FLY_ATTACK_H
#define FLY_ATTACK_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class FlyAttack : public EntityState
{
private:
	const float m_delay = 0.5f;
	float m_elapsedTime = 0.f;
public:
	FlyAttack(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !FLY_ATTACK_H
