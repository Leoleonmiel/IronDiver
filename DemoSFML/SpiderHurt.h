#ifndef SPIDER_HURT_H
#define SPIDER_HURT_H

#include "EntityState.h"

class SpiderHurt : public EntityState
{
private:
	const float m_duration = 1.5f;
	const float m_baseKnockback = 1250.0f;
	const float m_damageKnockback = 0.0f;

	float m_knockbackDirection = 0.f;
	int m_damage = 0;
	float m_elapsedTime = 0.f;
public:
	SpiderHurt(Entity* _owner, int _damage);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
