#include "CutsceneEntitySetState.h"
#include "Entity.h"

CutsceneEntitySetState::CutsceneEntitySetState(Entity* _entity, EntityState* _state)
{
	m_entity = _entity;
	m_state = _state;
}

void CutsceneEntitySetState::Init()
{
	m_entity->SetState(m_state);
}

bool CutsceneEntitySetState::InstructionDone()
{
	return true;
}
