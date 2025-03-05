#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <map>
#include <string>
#include <functional>
#include <queue>

#include "SFML/System/Clock.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

struct Animation;

typedef std::function<sf::Vector2f(void)> GetPositionFunc;

class Animator 
{
private:
	std::map<std::string, Animation*>* m_animations = nullptr;
	Animation* m_currentAnimation = nullptr;
	Animation* m_previousAnimation = nullptr;
	std::queue<Animation*> m_animationQueue;
	float m_timer = 0.f;
	float m_animatorSpeed = 0.f;

	int m_loopCount = 0;
	int m_currentFrame = 0;

	bool m_gameSpeedChange = true;

	GetPositionFunc* m_getPositionFunc = NULL;

	void NextAnimation();
	void DisplayFrame(int _frameIndex, sf::RenderTarget& _window);
public:
	Animator(GetPositionFunc* _func);
	~Animator();
	bool AddAnimation(std::string _name, Animation* _animation);
	bool TrySetCurrentAnimation(std::string _name);
	Animation* GetCurrentAnimation(void);
	std::string GetCurrentAnimationName(void);

	void SetAnimationQueue(std::queue<Animation*>& _animationQueue);

	void PlayAnimation(Animation* _animation, bool _resetClock = true);
	void PlayPreviousAnimation(bool _resetClock = true);

	int GetLoopCount() const { return m_loopCount; }
	bool LoopedOnce() const { return m_loopCount > 0; }
	int GetCurrentFrame() const;
	GetPositionFunc GetGetPositionFunc() { return *m_getPositionFunc; }

	bool TryGetAnimation(std::string _name, Animation*& _animation);
	bool TryGetAnimation(std::string _name);

	float GetAnimatorSpeed() const;
	void SetAnimatorSpeed(float _speed);
	void SetGameSpeedChange(bool _change) { m_gameSpeedChange = _change; }

	void Display(sf::RenderTarget& _window);

	void StopAnimation();

};

#endif // !ANIMATOR_H
