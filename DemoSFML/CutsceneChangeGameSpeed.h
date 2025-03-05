#ifndef CUTSCENE_CHANGE_GAME_SPEED_H
#define CUTSCENE_CHANGE_GAME_SPEED_H

#include "Cutscenes.h"


class CutsceneChangeGameSpeed : public CutsceneInstruction
{
private:
	float m_speed;
public:
	CutsceneChangeGameSpeed(float _speed);
	void Init() override;
	bool InstructionDone() override;
};

#endif