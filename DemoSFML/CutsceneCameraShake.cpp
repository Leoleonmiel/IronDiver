#include "CutsceneCameraShake.h"
#include "CameraManager.h"
#include "common.h"

CutsceneCameraShake::CutsceneCameraShake(float _angle, float _angleAmplitude, float _amplitude, float _duration)
{
	m_angle = _angle;
	m_angleAmplitude = _angleAmplitude;
	m_amplitude = _amplitude;
	m_duration = _duration;
}

void CutsceneCameraShake::Init()
{
	CameraManager::PrepareForShake(m_angle, m_angleAmplitude, m_amplitude);
	CameraManager::Shake(m_duration);
}

bool CutsceneCameraShake::InstructionDone()
{
	return true;
}