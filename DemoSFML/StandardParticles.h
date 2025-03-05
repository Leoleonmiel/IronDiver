#ifndef STANDARD_PARTICLES_H
#define STANDARD_PARTICLES_H

#include "ParticleManager.h"
#include "SFML/System/Vector2.hpp"
#include <functional>

class Animation;
class Animator;
typedef std::function<sf::Vector2f(void)> GetPositionFunc;

class StandardParticle : public ParticleBase
{
private:
	float m_lifeTime;
	float m_maxLifeTime;
	float m_gravity = 0.f;

	sf::Vector2f m_velocity;
	sf::Vector2f m_position;

	Animator* m_animator = nullptr;
	Animation* m_animation = nullptr;
public:
	StandardParticle(float _angle, float _speed, float _maxLifeTime, sf::Vector2f _basePos = sf::Vector2f {0,0}, Animation* _animation = nullptr);
	StandardParticle(float _angle, float _speed, float _maxLifeTime, GetPositionFunc* _funcPos, Animation* _animation);
	StandardParticle(sf::Vector2f _velocity, float _maxLifeTime, sf::Vector2f _basePos = sf::Vector2f{ 0,0 }, Animation* _animation = nullptr);
	~StandardParticle();
	void SetGravity(float _gravity);
	void Update() override;
	bool IsAlive() override;
	void Render(sf::RenderTarget& _target) override;
};

#endif