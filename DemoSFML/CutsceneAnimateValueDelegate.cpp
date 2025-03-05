#include "CutsceneAnimateValueDelegate.h"
#include "common.h"

CutsceneAnimateValueDelegate::CutsceneAnimateValueDelegate(float _value, float _baseValue, float _time, SetValueDelegate* _func, bool _deleteFuncAfter)
{
	m_value = _value;
	m_baseValue = _baseValue;
	m_baseTime = _time;
	m_time = _time;
	m_func = _func;
	m_deleteFuncAfter = _deleteFuncAfter;
}

void CutsceneAnimateValueDelegate::Init()
{
	
}

bool CutsceneAnimateValueDelegate::InstructionDone()
{
	m_time -= GameData::DeltaTime;
	if (m_time <= 0)
	{
		(*m_func)(m_value);
		if (m_deleteFuncAfter)
		{
			delete m_func;
		}
		return true;
	}
	float timePercent = 1 - (m_time / m_baseTime);
	float value = ((m_value - m_baseValue) * timePercent) + m_baseValue;
	(*m_func)(value);
	return false;
}