#ifndef CUTSCENE_CONTROL_FRAME_H
#define CUTSCENE_CONTROL_FRAME_H

#include "Cutscenes.h"
#include "SFML/Graphics.hpp"

class CutsceneControlFrame : public CutsceneInstruction
{
private:
	sf::RectangleShape** m_rect;
	bool m_rising;
	float m_timer;
	float m_duration;
	float m_height;

public:
	CutsceneControlFrame(sf::RectangleShape** _rect, bool _rising, float _duration, float _height);
	void Init() override;
	bool InstructionDone() override;
};

#endif