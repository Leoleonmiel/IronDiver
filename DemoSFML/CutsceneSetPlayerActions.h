#ifndef CUTSCENE_SET_PLAYER_ACTIONS_H
#define CUTSCENE_SET_PLAYER_ACTIONS_H

#include "Cutscenes.h"
#include "SFML/System/Vector2.hpp"

/// <summary>
/// Instruction used to make the camera go to and stay at a specific location
/// <para> InstructionDone() returns true when the camera has reached the goal </para>
/// </summary>
class CutsceneSetPlayerActions : public CutsceneInstruction
{
private:
	bool m_value;
public:
	CutsceneSetPlayerActions(bool);
	void Init() override;
	bool InstructionDone() override;
};

#endif