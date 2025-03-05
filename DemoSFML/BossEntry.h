#ifndef BOSS_ENTRY_H
#define BOSS_ENTRY_H

#include "EntityState.h"

class BossEntry : public EntityState
{
private:
	const float m_speed = 450.f;

public:
	BossEntry(Entity* _owner);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif // !
