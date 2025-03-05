#pragma once
#include "EntityState.h"

class Character;

class EntityRunawayFlying : public EntityState
{
private:
	float m_speed = 0.f;
	
public:
	EntityRunawayFlying(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

