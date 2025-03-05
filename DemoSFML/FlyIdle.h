#ifndef FLY_IDLE_H
#define FLY_IDLE_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"
#include "SFML/System/Vector2.hpp"

class FlyIdle : public EntityState
{
private:
	float m_elapsedTime = 0.f;
	bool m_hitboxDead = false;

public:
	FlyIdle(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !

