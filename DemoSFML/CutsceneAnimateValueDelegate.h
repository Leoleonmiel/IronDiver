#ifndef CUTSCENE_ANIMATE_VALUE_DELEGATE_H
#define CUTSCENE_ANIMATE_VALUE_DELEGATE_H

#include "Cutscenes.h"
#include <functional>

typedef std::function<void(float)> SetValueDelegate;

class CutsceneAnimateValueDelegate : public CutsceneInstruction
{
private:
	float m_value;
	float m_baseValue;
	float m_baseTime;
	float m_time;
	bool m_deleteFuncAfter;

	SetValueDelegate* m_func;
public:
	CutsceneAnimateValueDelegate(float _value, float _baseValue, float _time, SetValueDelegate* _func, bool _deleteFuncAfter);
	void Init() override;
	bool InstructionDone() override;
};

#endif // !CUTSCENE_ANIMATE_VALUE_DELEGATE_H
