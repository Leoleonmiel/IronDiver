#ifndef CUTSCENE_ZOOM_H
#define CUTSCENE_ZOOM_H

#include "Cutscenes.h"

class CutsceneZoom : public CutsceneInstruction
{
private:
	float m_zoom;
	float m_zoomSpeed;
public:
	CutsceneZoom(float _zoom, float _zoomSpeed);

	void Init() override;
	bool InstructionDone() override;
};

#endif