#ifndef CUTSCENE_CAMERA_RESET_H
#define CUTSCENE_CAMERA_RESET_H

#include "Cutscenes.h"

/// <summary>
/// Instruction used to make the camera go back to the player
/// <para> InstructionDone() returns true once the camera is at the player's location</para>
/// </summary>
class CutsceneCameraReset : public CutsceneInstruction
{
private:
	float m_duration;
	float m_timer;
public:
	CutsceneCameraReset();
	void Init() override;
	bool InstructionDone() override;
};

#endif // !CUTSCENE_CAMERA_RESET_H
