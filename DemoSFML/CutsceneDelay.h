#ifndef CUTSCENE_DELAY_H
#define CUTSCENE_DELAY_H

#include "Cutscenes.h"
#include "SFML/System/Clock.hpp"

/// <summary>
/// Instruction used to introduce a delay in the cutscene
/// <para> Useful to not lose the player </para>
/// </summary>
class CutsceneDelay : public CutsceneInstruction
{
private:
	float m_seconds;
	float m_timer;
public:
	CutsceneDelay(float seconds);
	void Init() override;
	bool InstructionDone() override;
};

#endif // !CUTSCENE_DELAY_H
