#include "TimeSpeedManager.h"
#include "common.h"

float TimeSpeedManager::m_goalSpeed = 1.f;
float TimeSpeedManager::m_defaultSpeed = 1.f;
float TimeSpeedManager::m_currentSpeed = 1.f;

float TimeSpeedManager::m_breakSpeedReturnTime = 0.f;

float TimeSpeedManager::m_breakDuration = 0.f;
float TimeSpeedManager::m_breakTime = 0.f;
float TimeSpeedManager::m_breakSpeed = 0.f;
float TimeSpeedManager::m_previousSpeed = 1.f;

void TimeSpeedManager::Update(void)
{
	if (m_breakTime > 0.f)
	{
		m_breakTime -= GameData::DeltaTime;
		float timeElapsed = m_breakSpeedReturnTime + m_breakDuration - m_breakTime;
		
		if (m_breakTime <= 0.f)
		{
			GameData::GameSpeed = m_previousSpeed;
		}
		else
		{
			if (timeElapsed <= m_breakDuration)
			{
				GameData::GameSpeed = m_breakSpeed;
			}
			else
			{
				timeElapsed -= m_breakDuration;
				float value = timeElapsed / m_breakSpeedReturnTime;
				value *= value;
				GameData::GameSpeed = m_breakSpeed + (m_previousSpeed - m_breakSpeed) * value;
			}
		}
	}
}

void TimeSpeedManager::SetBreakTime(float _time, float _riseTime, float _previousSpeed)
{
	m_previousSpeed = _previousSpeed;
	m_breakTime = _time + _riseTime;
	m_breakSpeedReturnTime = _riseTime;
	m_breakDuration = _time;
}

void TimeSpeedManager::SetBreakSpeed(float _speed)
{
	m_breakSpeed = _speed;
}


