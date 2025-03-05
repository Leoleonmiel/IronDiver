#include "CutsceneBooleanLambda.h"

CutsceneBooleanLambda::CutsceneBooleanLambda(GetBooleanLambda _func)
{
	m_func = _func;
}

void CutsceneBooleanLambda::Init()
{

}

bool CutsceneBooleanLambda::InstructionDone()
{
	return m_func();
}
