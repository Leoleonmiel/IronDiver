#include "CutsceneGetStateEnd.h"

#include "Entity.h"

CutsceneWaitForStateEnd::CutsceneWaitForStateEnd(Entity* _entity)
{
	m_entity = _entity;
}

void CutsceneWaitForStateEnd::Init()
{
	m_state = m_entity->GetState();
}

bool CutsceneWaitForStateEnd::InstructionDone()
{
	return m_entity->GetState() != m_state;
}