#ifndef CUTSCENE_DIALOG_H
#define CUTSCENE_DIALOG_H

#include "Cutscenes.h"

class Dialog;

/// <summary>
/// Instruction used to show a dialog
/// <para> InstructionDone() returns when the dialog is passed (player input) </para>
/// </summary>
class CutsceneDialog : public CutsceneInstruction
{
private:
	Dialog* m_dialog;
	float m_timer;
public:
	CutsceneDialog(Dialog* _dialog);
	void Init() override;
	bool InstructionDone() override;
};

#endif // !CUTSCENE_DIALOG_H
