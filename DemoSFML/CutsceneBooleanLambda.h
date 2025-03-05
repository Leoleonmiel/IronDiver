#ifndef CUTSCENE_BOOLEAN_LAMBDA_H
#define CUTSCENE_BOOLEAN_LAMBDA_H

#include "Cutscenes.h"
#include <functional>

typedef std::function<bool()> GetBooleanLambda;

class CutsceneBooleanLambda : public CutsceneInstruction
{
private:
	GetBooleanLambda m_func;
public:
	CutsceneBooleanLambda(GetBooleanLambda _func);
	void Init() override;
	bool InstructionDone() override;
};

#endif // CUTSCENE_BOOLEAN_LAMBDA_H
