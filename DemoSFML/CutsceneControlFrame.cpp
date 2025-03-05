#include "CutsceneControlFrame.h"
#include "common.h"

CutsceneControlFrame::CutsceneControlFrame(sf::RectangleShape** _rect, bool _rising, float _duration, float _height)
{
	m_rect = _rect;
	m_rising = _rising;
	m_duration = _duration;
	m_height = _height;
}

void CutsceneControlFrame::Init()
{
	m_timer = 0.f;
}

bool CutsceneControlFrame::InstructionDone()
{
	float ratio;
	m_timer += GameData::DeltaTime;
	ratio = m_timer / m_duration;
	if (ratio > 1)
	{
		ratio = 1;
	}

	if (m_rising)
	{
		ratio = 1 - ratio;
	}
	(*m_rect)[0].setPosition({0, m_height * -ratio});
	(*m_rect)[1].setPosition({0, SCREEN_SIZE.y - m_height * (1 - ratio)});

	if (m_timer >= m_duration)
	{
		return true;
	}

	return false;
}
