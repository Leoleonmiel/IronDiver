#ifndef CUTSCENES_H
#define CUTSCENES_H

#include <queue>

/// <summary>
/// Base class for any Cutscene Instruction
/// <para> Each instruction should override Init() and InstructionDone() </para>
/// <para> InstructionDone() should return true if the instruction is done </para>
/// <para> In case we need an update, please use InstructionDone as it is called every frame </para>
/// </summary>
class CutsceneInstruction
{
public:
	virtual void Init() = 0;
	virtual bool InstructionDone() = 0;
};

/// <summary>
/// Static Class that handles order of execution of Cutscene Instructions
/// </summary>
class CutsceneMaster
{
public:
	static void SetInstructions(std::queue<CutsceneInstruction*>& _instructions);
	static void AddInstructions(std::queue<CutsceneInstruction*>& _instructions);
	static void AddInstruction(CutsceneInstruction* _instruction);
	static bool InstructionsDone() { return m_instructions.size() == 0; }

	static void Update();
private:
	static std::queue<CutsceneInstruction*> m_instructions;
	static CutsceneInstruction* m_currentInstruction;
};

#endif // !CUTSCENES_H
