#include "CutsceneCameraGoTo.h"
#include "CameraManager.h"
#include "common.h"

CutsceneCameraGoTo::CutsceneCameraGoTo(sf::Vector2f _goal)
{
	m_goal = _goal;
}

void CutsceneCameraGoTo::Init()
{
	CameraManager::SetFixedCameraTarget(m_goal);
	CameraManager::SetCameraFixed(true);
}

bool CutsceneCameraGoTo::InstructionDone()
{
	return CameraManager::GetInstance()->ReachedGoal();
}
