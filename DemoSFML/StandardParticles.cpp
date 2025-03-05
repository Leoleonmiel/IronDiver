#include "StandardParticles.h"
#include "common.h"
#include "SFML/Graphics.hpp"
#include "Animator.h"
#include "Animation.h"

StandardParticle::StandardParticle(float _angle, float _speed, float _maxLifeTime, sf::Vector2f _basePos, Animation* _animation)
{
	sf::Vector2f velocity;
	velocity = { cos(_angle), sin(_angle) };
	velocity *= _speed;

	m_maxLifeTime = _maxLifeTime;
	m_lifeTime = _maxLifeTime;

	m_velocity = velocity;
	m_position = _basePos;

	GetPositionFunc* func = new GetPositionFunc;
	*func = [this](void) ->sf::Vector2f {return m_position; };
	m_animator = new Animator(func);
	m_animation = _animation;
	m_animator->SetGameSpeedChange(false);
	m_animator->PlayAnimation(m_animation);
}

StandardParticle::StandardParticle(float _angle, float _speed, float _maxLifeTime, GetPositionFunc* _funcPos, Animation* _animation)
{
	sf::Vector2f velocity;
	velocity = { cos(_angle), sin(_angle)};
	velocity *= _speed;

	m_maxLifeTime = _maxLifeTime;
	m_lifeTime = m_maxLifeTime;

	m_velocity = velocity;
	m_position = (*_funcPos)();

	m_animator = new Animator(_funcPos);
	m_animation = _animation;
	m_animator->SetGameSpeedChange(false);
	m_animator->PlayAnimation(m_animation);
}

StandardParticle::StandardParticle(sf::Vector2f _velocity, float _maxLifeTime, sf::Vector2f _basePos, Animation* _animation)
{
}

StandardParticle::~StandardParticle()
{
	delete m_animation->sprite;
	delete m_animation;
	delete m_animator;
}


void StandardParticle::SetGravity(float _gravity)
{
	m_gravity = _gravity;
}

void StandardParticle::Update()
{
	float dt = GameData::DeltaTime * GameData::GameSpeed;

	if (m_gravity != 0)
	{
		m_velocity.y += m_gravity * dt;
	}

	m_position += m_velocity * dt;

	m_lifeTime -= dt;
}

bool StandardParticle::IsAlive()
{
	return !m_animator->LoopedOnce();
}

void StandardParticle::Render(sf::RenderTarget& _target)
{
	m_animator->Display(_target);
}
