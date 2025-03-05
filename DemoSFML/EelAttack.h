#ifndef EEL_ATTACK_H
#define EEL_ATTACK_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class EelAttack : public EntityState
{
private:
	const float m_duration = 1.5f;
	float m_elapsedTime = 0.f;
public:
	EelAttack(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
