#ifndef CUTSCENE_CAMERA_GO_TO_H
#define CUTSCENE_CAMERA_GO_TO_H

#include "Cutscenes.h"
#include "SFML/System/Vector2.hpp"

/// <summary>
/// Instruction used to make the camera go to and stay at a specific location
/// <para> InstructionDone() returns true when the camera has reached the goal </para>
/// </summary>
class CutsceneCameraGoTo : public CutsceneInstruction
{
private:
	sf::Vector2f m_goal;
public:
	CutsceneCameraGoTo(sf::Vector2f);
	void Init() override;
	bool InstructionDone() override;
};

#endif
