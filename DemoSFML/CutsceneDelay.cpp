#include "CutsceneDelay.h"
#include "common.h"

CutsceneDelay::CutsceneDelay(float _seconds)
{
	m_seconds = _seconds;
}

void CutsceneDelay::Init()
{
	m_timer = m_seconds;
}

bool CutsceneDelay::InstructionDone()
{
	m_timer -= GameData::DeltaTime * GameData::GameSpeed;

	if (m_timer <= 0)
	{
		return true;
	}

	return false;
}