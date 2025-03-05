#include "CutsceneSetValueDelegate.h"

CutsceneSetValueDelegate::CutsceneSetValueDelegate(float _value, SetValueDelegate* _func, bool _deleteFuncAfter)
{
	m_value = _value;
	m_func = _func;
	m_deleteFuncAfter = _deleteFuncAfter;
}

void CutsceneSetValueDelegate::Init()
{
	(*m_func)(m_value);
}

bool CutsceneSetValueDelegate::InstructionDone()
{
	if (m_deleteFuncAfter)
	{
		delete m_func;
	}

	return true;
}
