#include "CutsceneZoom.h"
#include "CameraManager.h"

CutsceneZoom::CutsceneZoom(float _zoom, float _zoomSpeed)
{
	m_zoom = _zoom;
	m_zoomSpeed = _zoomSpeed;
}

void CutsceneZoom::Init()
{
	CameraManager* camera = CameraManager::GetInstance();
	camera->SetZoomGoal(m_zoom);
	camera->SetZoomSpeed(m_zoomSpeed);
}

bool CutsceneZoom::InstructionDone()
{
	return CameraManager::GetInstance()->ReachedZoomGoal();
}