#pragma once
class Entity;

class EntityState
{
protected:
	Entity* m_owner;

public:
	EntityState(Entity*);
	virtual void InitState() = 0;
	virtual void Update() = 0;
	virtual void ExitState() = 0;
	virtual EntityState* GetNextState() = 0;
};

