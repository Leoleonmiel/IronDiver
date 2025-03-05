#include "CutsceneSetPlayerActions.h"
#include "common.h"
#include "CharacterController.h"

CutsceneSetPlayerActions::CutsceneSetPlayerActions(bool _value) : CutsceneInstruction()
{
	m_value = _value;
}

void CutsceneSetPlayerActions::Init()
{
	GameData::CharaCtrl->SetCanControl(m_value);
	GameData::CharaCtrl->SetState(GameData::CharaCtrl->WAIT);
}

bool CutsceneSetPlayerActions::InstructionDone()
{
	return true;
}


