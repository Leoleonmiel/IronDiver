#ifndef CUTSCENE_ENTITY_SET_STATE_H
#define CUTSCENE_ENTITY_SET_STATE_H

#include "Cutscenes.h"

class Entity;
class EntityState;

class CutsceneEntitySetState : public CutsceneInstruction
{
private:
	Entity* m_entity;
	EntityState* m_state;

public:
	CutsceneEntitySetState(Entity* _entity, EntityState* _state);
	void Init() override;
	bool InstructionDone() override;
};

#endif // CUTSCENE_ENTITY_SET_STATE_H
