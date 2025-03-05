#ifndef CUTSCENE_SET_VALUE_TEMPLATE
#define CUTSCENE_SET_VALUE_TEMPLATE

#include "Cutscenes.h"

typedef std::function<void()> SetValueLambda;

class CutsceneSetValueLambda : public CutsceneInstruction
{
private:
	SetValueLambda m_lambda;

public:
	CutsceneSetValueLambda(SetValueLambda _lambda)
	{
		m_lambda = _lambda;
	}

	void Init() override;
	bool InstructionDone() override;
};
#endif // CUTSCENE_SET_VALUE_TEMPLATE

inline void CutsceneSetValueLambda::Init()
{
	m_lambda();
}

inline bool CutsceneSetValueLambda::InstructionDone()
{
	return true;
}
