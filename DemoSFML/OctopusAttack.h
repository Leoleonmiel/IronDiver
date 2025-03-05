#ifndef OCTOPUS_ATTACK_H
#define OCTOPUS_ATTACK_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class OctopusAttack : public EntityState
{
private:
	const float m_duration = 1.5f;
	float m_elapsedTime = 0.f;
public:
	OctopusAttack(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !OCTOPUS_ATTACK_H
