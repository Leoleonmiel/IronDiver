#include "CutsceneChangeGameSpeed.h"
#include "common.h"

CutsceneChangeGameSpeed::CutsceneChangeGameSpeed(float _speed)
{
	m_speed = _speed;
}

void CutsceneChangeGameSpeed::Init()
{
	GameData::GameSpeed = m_speed;
}

bool CutsceneChangeGameSpeed::InstructionDone()
{
	return true;
}
