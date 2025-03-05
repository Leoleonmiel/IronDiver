#include "CutsceneDialogDisable.h"
#include "DialogModule.h"

void CutsceneDialogDisable::Init()
{
	DialogMaster::SetEnabled(false);
}

bool CutsceneDialogDisable::InstructionDone()
{
	return true;
}


