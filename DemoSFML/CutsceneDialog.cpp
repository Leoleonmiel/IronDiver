#include "CutsceneDialog.h"
#include "DialogModule.h"
#include <iostream>

CutsceneDialog::CutsceneDialog(Dialog* _dialog)
{
	m_dialog = _dialog;
	m_timer = 1.275f;
}

void CutsceneDialog::Init()
{
	DialogMaster::SetEnabled(true);
	Dialog dialog;
	DialogMaster::TrySetDialog(*m_dialog);
}

bool CutsceneDialog::InstructionDone()
{
	return !DialogMaster::DialogActive();
}
