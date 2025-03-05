#include <iostream>

#include "Animator.h"
#include "Animation.h"
#include "common.h"


void Animator::NextAnimation()
{
	m_currentAnimation = m_animationQueue.front();
	m_animationQueue.pop();
	m_currentAnimation->sprite->setTexture(m_currentAnimation->textureList[0]);
	m_loopCount = 0;

	if (m_currentAnimation->resetTimerBefore)
	{
		m_timer = 0.f;
	}
}

void Animator::DisplayFrame(int _frameIndex, sf::RenderTarget& _window)
{
	sf::Vector2f pos = m_getPositionFunc == nullptr ? sf::Vector2f{0, 0} : (*m_getPositionFunc)();

	if (m_currentAnimation->firstFrame.width == 425)
	{
		std::cout << "Position is " << pos.x << " " << pos.y << std::endl;
	}

	m_currentFrame = _frameIndex;

	m_currentAnimation->sprite->setTextureRect({ m_currentAnimation->firstFrame.left + (_frameIndex * m_currentAnimation->firstFrame.width), m_currentAnimation->firstFrame.top, m_currentAnimation->firstFrame.width, m_currentAnimation->firstFrame.height });
	m_currentAnimation->sprite->setPosition(pos);
	_window.draw(*m_currentAnimation->sprite);
}

Animator::Animator(GetPositionFunc* _func)
{
	m_animations = new std::map<std::string, Animation*>;
	m_currentAnimation = nullptr;
	m_previousAnimation = nullptr;
	m_timer = 0.f;
	m_animatorSpeed = 1.0f;	
	m_getPositionFunc = _func;
	m_loopCount = 0;
}

Animator::~Animator()
{
	delete m_getPositionFunc;
	m_getPositionFunc = nullptr;
}

bool Animator::AddAnimation(std::string _name, Animation* _animation)
{
	if (m_animations->find(_name) != m_animations->end())
	{
		return false;
	}

	m_animations->insert(std::pair<std::string, Animation*>(_name, _animation));
	if (m_currentAnimation == nullptr)
	{
		TrySetCurrentAnimation(_name);
	}
	return true;
}

bool Animator::TrySetCurrentAnimation(std::string _name)
{
	Animation* anim = NULL;
	if (m_animations->find(_name) == m_animations->end())
	{
		return false;
	}

	while (!m_animationQueue.empty())
	{
		m_animationQueue.pop();
	}

	m_animationQueue.push(m_animations->at(_name));
	NextAnimation();

	return true;
}

Animation* Animator::GetCurrentAnimation(void)
{
	return m_currentAnimation;
}

std::string Animator::GetCurrentAnimationName(void)
{
	for (auto it = m_animations->begin(); it != m_animations->end(); it++)
	{
		if ((*it).second == m_currentAnimation)
		{
			return (*it).first;
		}
	}

	return "";
}

void Animator::SetAnimationQueue(std::queue<Animation*>& _animationQueue)
{
	if (_animationQueue.empty())
	{
		while (!m_animationQueue.empty())
		{
			m_animationQueue.pop();
		}

		return;
	}

	m_animationQueue = _animationQueue;
	NextAnimation();
}

void Animator::PlayAnimation(Animation* _animation, bool _resetClock)
{
	m_previousAnimation = m_currentAnimation;
	m_currentAnimation = _animation;

	if (m_currentAnimation != nullptr)
	{
		m_currentAnimation->sprite->setTexture(m_currentAnimation->textureList[0]);
	}
	

	m_loopCount = 0;

	if (_resetClock)
	{
		m_timer = 0.f;
	}
}

void Animator::PlayPreviousAnimation(bool _resetClock)
{
	m_currentAnimation = m_previousAnimation;
	m_previousAnimation = nullptr;
	
	m_currentAnimation->sprite->setTexture(m_currentAnimation->textureList[0]);

	m_loopCount = 0;

	if (_resetClock)
	{
		m_timer = 0.f;
	}
}

int Animator::GetCurrentFrame() const
{
	return m_currentFrame;
}

bool Animator::TryGetAnimation(std::string _name, Animation*& _animation)
{
	if (m_animations->find(_name) != m_animations->end())
	{
		_animation = m_animations->at(_name);
		return true;
	}
	return false;
}

bool Animator::TryGetAnimation(std::string _name)
{
	if (m_animations->find(_name) != m_animations->end())
	{
		return true;
	}
	return false;
}

float Animator::GetAnimatorSpeed() const
{
	return m_animatorSpeed;
}

void Animator::SetAnimatorSpeed(float _speed)
{
	m_animatorSpeed = _speed;
}

void Animator::Display(sf::RenderTarget& _window)
{

	if (m_currentAnimation == nullptr)
	{
		return;
	}
	
	unsigned frameIndex;
	float dt = GameData::DeltaTime * m_animatorSpeed;
	if (m_gameSpeedChange)
	{
		dt *= GameData::GameSpeed;
	}
	m_timer += dt;

	
	if (m_timer < m_currentAnimation->nbFrame / m_currentAnimation->speed)
	{
		frameIndex = (unsigned)std::floor(m_timer / (1 / m_currentAnimation->speed));
		DisplayFrame(frameIndex, _window);
		return;
	}

	if (!m_currentAnimation->loop)
	{
		if (!m_animationQueue.empty())
		{
			NextAnimation();
		}
		else
		{
			m_loopCount = 1;
		}
		
		frameIndex = (unsigned)std::floor(m_timer / (1 / m_currentAnimation->speed));
		if (frameIndex >= m_currentAnimation->nbFrame)
		{
			frameIndex = m_currentAnimation->nbFrame - 1;
		}
		DisplayFrame(frameIndex, _window);
		return;
	}

	if (m_currentAnimation->loopCount == -1 || m_loopCount < m_currentAnimation->loopCount)
	{
		m_timer -= m_currentAnimation->nbFrame / m_currentAnimation->speed;
		frameIndex = (unsigned)std::floor(m_timer / (1 / m_currentAnimation->speed));
		if (frameIndex >= m_currentAnimation->nbFrame)
		{
			frameIndex = m_currentAnimation->nbFrame - 1;
		}
		m_loopCount++;
		DisplayFrame(frameIndex, _window);
		return;
	}

	if (!m_animationQueue.empty())
	{
		NextAnimation();
	}

	frameIndex = (unsigned)std::floor(m_timer / (1 / m_currentAnimation->speed));
	if (frameIndex >= m_currentAnimation->nbFrame)
	{
		frameIndex = m_currentAnimation->nbFrame - 1;
	}
	DisplayFrame(frameIndex, _window);
}

void Animator::StopAnimation()
{
	m_currentAnimation = nullptr;
	m_previousAnimation = nullptr;
	m_animationQueue = std::queue<Animation*>();
	m_timer = 0.f;
	m_animatorSpeed = 1.0f;
	m_loopCount = 0;

	for (size_t i = 0; i < m_animationQueue.size(); i++)
		m_animationQueue.pop();
}
