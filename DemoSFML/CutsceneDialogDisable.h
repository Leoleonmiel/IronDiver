#ifndef CUTSCENE_DIALOG_DISABLE_H
#define CUTSCENE_DIALOG_DISABLE_H

#include "Cutscenes.h"

/// <summary>
/// Instruction used to stop rendering the Dialogue GUI
/// </summary>
class CutsceneDialogDisable : public CutsceneInstruction
{
public:
	void Init() override;
	bool InstructionDone() override;
};

#endif // !CUTSCENE_DIALOG_DISABLE_H
