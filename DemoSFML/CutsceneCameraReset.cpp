#include "CutsceneCameraReset.h"
#include "CameraManager.h"
#include "common.h"
#include "CharacterController.h"

void CutsceneCameraReset::Init()
{
	m_timer = 0.f;
	CameraManager::SetCameraFixed(false);
	CameraManager& camera = *CameraManager::GetInstance();
	camera.ResetLookAhead();
}

CutsceneCameraReset::CutsceneCameraReset()
{
}

bool CutsceneCameraReset::InstructionDone()
{
	CameraManager& camera = *CameraManager::GetInstance();

	camera.Update();

	if (camera.ReachedGoal())
	{
		return true;
	}

	return false;
}
