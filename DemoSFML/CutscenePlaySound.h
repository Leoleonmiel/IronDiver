#ifndef CUTSCENE_PLAY_SOUND_H
#define CUTSCENE_PLAY_SOUND_H

#include "Cutscenes.h"
#include "SFML/System/Clock.hpp"
#include <string>

/// <summary>
/// Instruction used to play a sound
/// </summary>
class CutscenePlaySound : public CutsceneInstruction
{
private:
	std::string m_SoundName;
public:
	CutscenePlaySound(std::string _name);
	void Init() override;
	bool InstructionDone() override;
};

#endif // !CUTSCENE_DELAY_H
