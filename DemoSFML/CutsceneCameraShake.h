#ifndef CUTSCENE_CAMERA_SHAKE_H
#define CUTSCENE_CAMERA_SHAKE_H

#include "Cutscenes.h"

class CutsceneCameraShake : public CutsceneInstruction
{
private:
	float m_angle;
	float m_angleAmplitude;
	float m_amplitude;
	float m_duration;

public:
	CutsceneCameraShake(float _angle, float _angleAmplitude, float _amplitude, float _duration);
	void Init() override;
	bool InstructionDone() override;
};

#endif