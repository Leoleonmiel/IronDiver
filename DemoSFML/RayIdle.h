#ifndef RAY_IDLE_H
#define RAY_IDLE_H

#include "EntityState.h"

class RayIdle : public EntityState
{
private:
	bool m_goingRight = 0.f;
	float m_duration = 0.f;
	float m_timer = 0.f;

	const float m_speed = 200.0f;
public:
	RayIdle(Entity* _owner, bool _right, float _duration);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !RAY_IDLE_H
