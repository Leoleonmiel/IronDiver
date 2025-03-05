#ifndef CUTSCENE_PLAY_ANIMATION_H
#define CUTSCENE_PLAY_ANIMATION_H

#include <functional>
#include "Cutscenes.h"
struct Animation;
class Animator;

class CutscenePlayAnimation : public CutsceneInstruction
{
private:
	Animation* m_animation;
	Animator* m_animator;
	bool m_deleteAnimationAfterPlayback;
public:
	CutscenePlayAnimation(Animator* _animator, Animation* _animation, bool _deleteAnimationAfterPlayback);
	void Init() override;
	bool InstructionDone() override;
};

#endif // !CUTSCENE_PLAY_ANIMATION_H
