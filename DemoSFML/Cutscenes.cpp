#include "Cutscenes.h"
#include <iostream>

std::queue<CutsceneInstruction*> CutsceneMaster::m_instructions;
CutsceneInstruction* CutsceneMaster::m_currentInstruction;

void CutsceneMaster::SetInstructions(std::queue<CutsceneInstruction*>& _instructions)
{
	while (m_instructions.size() > 0)
	{
		delete m_instructions.front();
		m_instructions.pop();
	}

	while (_instructions.size() > 0)
	{
		m_instructions.push(_instructions.front());
		_instructions.pop();
	}
}

void CutsceneMaster::AddInstructions(std::queue<CutsceneInstruction*>& _instructions)
{
	while (_instructions.size() > 0)
	{
		m_instructions.push(_instructions.front());
		_instructions.pop();
	}
}

void CutsceneMaster::AddInstruction(CutsceneInstruction* _instruction)
{
	m_instructions.push(_instruction);
}

void CutsceneMaster::Update()
{
	if (m_currentInstruction == nullptr && m_instructions.empty())
	{
		return;
	}

	if (m_currentInstruction == nullptr)
	{
		m_currentInstruction = m_instructions.front();
		m_instructions.pop();
		m_currentInstruction->Init();
	}

	while (m_currentInstruction->InstructionDone())
	{
		delete m_currentInstruction;
		if (m_instructions.empty())
		{
			m_currentInstruction = nullptr;
			return;
		}
		m_currentInstruction = m_instructions.front();
		m_instructions.pop();
		m_currentInstruction->Init();
	}
}
