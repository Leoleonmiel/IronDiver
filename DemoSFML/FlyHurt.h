#ifndef FLY_HURT_H
#define FLY_HURT_H

#include "EntityState.h"

class FlyHurt : public EntityState
{
private:
	const float m_duration = 1.5f;
	const float m_baseKnockback = 1250.0f;
	const float m_damageKnockback = 0.0f;

	float m_knockbackDirection;
	int m_damage;
	float m_elapsedTime = 0.f;
	bool m_hitboxDead = false;
public:
	FlyHurt(Entity* _owner, int _damage);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
