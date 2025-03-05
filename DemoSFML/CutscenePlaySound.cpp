#include "CutscenePlaySound.h"
#include "SoundManager.h"

CutscenePlaySound::CutscenePlaySound(std::string _name)
{
	m_SoundName = _name;
}

void CutscenePlaySound::Init()
{
	SoundManager::GetInstance()->Play(m_SoundName);
}

bool CutscenePlaySound::InstructionDone()
{
	return true;
}
