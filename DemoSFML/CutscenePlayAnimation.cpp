#include "CutscenePlayAnimation.h"
#include "Animation.h"
#include "Animator.h"

CutscenePlayAnimation::CutscenePlayAnimation(Animator* _animator, Animation* _animation, bool _deleteAnimationAfterPlayback)
{
	m_animation = _animation;
	m_deleteAnimationAfterPlayback = _deleteAnimationAfterPlayback;
	m_animator = _animator;
}

void CutscenePlayAnimation::Init()
{
	m_animator->PlayAnimation(m_animation);
}

bool CutscenePlayAnimation::InstructionDone()
{
	if (m_animator->LoopedOnce())
	{
		if (m_deleteAnimationAfterPlayback)
		{
			delete m_animation;
		}

		m_animator->PlayPreviousAnimation();
		return true;
	}

	return false;
}