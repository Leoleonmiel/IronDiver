#ifndef CUTSCENE_SET_VALUE_DELEGATE_H
#define CUTSCENE_SET_VALUE_DELEGATE_H

#include "Cutscenes.h"
#include <functional>

typedef std::function<void(float)> SetValueDelegate;

class CutsceneSetValueDelegate : public CutsceneInstruction
{
private:
	float m_value;
	bool m_deleteFuncAfter;

	SetValueDelegate* m_func;
public:
	CutsceneSetValueDelegate(float _value, SetValueDelegate* _func, bool _deleteFuncAfter);
	void Init() override;
	bool InstructionDone() override;
};

#endif // !CUTSCENE_ANIMATE_VALUE_DELEGATE_H