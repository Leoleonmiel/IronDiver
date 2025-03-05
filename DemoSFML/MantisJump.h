#ifndef MANTIS_JUMP_H
#define MANTIS_JUMP_H

#include "EntityState.h"
#include "SFML/System/Clock.hpp"

class MantisJump : public EntityState
{
private:
	const float m_jumpHeight = 25.0f;
	const float m_jumpTime = 2.f;
	const float m_jumpWidth = 15.0f;
	const float m_jumpSpeed = 25.0f;

	float m_elapsedTime = 0.f;
public:
	MantisJump(Entity*);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};


#endif // !

