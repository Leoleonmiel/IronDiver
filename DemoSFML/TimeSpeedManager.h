#ifndef TIME_SPEED_MANAGER_H
#define TIME_SPEED_MANAGER_H

class TimeSpeedManager
{
public:
	static void Update();
	static void SetBreakTime(float _time, float _riseTime, float _previousSpeed = 1.f);
	static void SetBreakSpeed(float _speed);
private:
	static float m_goalSpeed;
	static float m_defaultSpeed;
	static float m_currentSpeed;

	static float m_breakSpeedReturnTime;

	static float m_breakDuration;
	static float m_breakTime;
	static float m_breakSpeed;
	static float m_previousSpeed;
};

#endif