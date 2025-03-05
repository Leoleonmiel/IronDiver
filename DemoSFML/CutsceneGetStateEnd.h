#ifndef CUTSCENE_GET_STATE_END_H
#define CUTSCENE_GET_STATE_END_H

#include "Cutscenes.h"

class EntityState;
class Entity;

class CutsceneWaitForStateEnd : public CutsceneInstruction
{
private:
	EntityState* m_state;
	Entity* m_entity;
public:
	CutsceneWaitForStateEnd(Entity* _entity);
	void Init() override;
	bool InstructionDone() override;
};

#endif // !
