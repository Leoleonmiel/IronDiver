#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <Thor/Particles.hpp>
#include <Thor/Input.hpp>
#include <Thor/Animations.hpp>
#include <Thor/Config.hpp>
#include <Thor/Graphics.hpp>
#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>
#include <Thor/Time.hpp>
#include <Thor/Vectors.hpp>
#include <iostream>

typedef std::function<sf::Vector2f(void)> EmitterPosFunc;

class ParticleEmitter;
struct ColorGradient;

class ParticleSystem
{
public:

	enum AnimMode
	{
		ANIM_MODE_UNSELECTED = -1,
		LIFE_TIME,
		ONE_SHOT,
		LOOP
	};

	ParticleSystem(sf::Texture* _texture);
	~ParticleSystem() = default;

	void AddEmitter(ParticleEmitter* _emitter);
	void AddTextureRect(sf::IntRect _rect);
	void AddColorGradiant(ColorGradient& _colorGradiant);

	void Update(float _dt);
	void Display(sf::RenderWindow& _window);

	void ClearAffectors();

	void AnimParticles(AnimMode _animMode, float _framerate, int _nbFrames);
	void FadeParticles(float _inRatio, float _outRatio);
	void TorqueParticles(float _torque);
	void ScaleParticles(float _scaleX, float _scaleY);
	void ApplyForceOnParticles(float _forceX, float _forceY);

private:

	void Animation(thor::Particle& _particle, float _progress);

	thor::ParticleSystem* m_system = nullptr;
	std::vector<ParticleEmitter*> m_emittersList;
	AnimMode m_animMode = ANIM_MODE_UNSELECTED;

	float m_framerate = 0.f;
	unsigned m_nbFrames = 0;
};

class ParticleEmitter
{
public:
	ParticleEmitter(float _emissionRate, float _lifeTime);
	~ParticleEmitter();

	void Start();
	void Stop();

	void EmitRandomFrames(int _frameMin, int _frameMax);

	void SetPosition(EmitterPosFunc* _posFunc);
	void SetPosition(sf::Vector2f _pos);

	void SetRandomPosition(EmitterPosFunc* _posFuncA, EmitterPosFunc* _posFuncB);
	void SetRandomPosition(sf::Vector2f _posA, sf::Vector2f _posB);

	void SetRotationRad(float _angleRad);
	void SetRotationDeg(float _angleDeg);

	void SetRotationRandomRad(float _angleMinRad, float _angleMaxRad);
	void SetRotationRandomDeg(float _angleMinDeg, float _angleMaxDeg);

	void SetRotationSpeed(float _speed);

	void SetColor(sf::Color _color);
	void SetRandomColor(int _min, int _max);

	void SetStartVelocity(float _radius, float _angleDeg);
	void SetDeflect(float _deflect);

	void SetEmissionRate(float _emissionRate);


private:

	friend class ParticleSystem;

	sf::Vector2f GetRandomPos(sf::Vector2f _posA, sf::Vector2f _posB);
	sf::Color GetRandomColor();

	EmitterPosFunc* m_posFuncA = nullptr;
	EmitterPosFunc* m_posFuncB = nullptr;

	sf::Vector2f m_posA;
	sf::Vector2f m_posB;

	thor::UniversalEmitter* m_emitter = nullptr;

	sf::Vector2i m_colorRandArea;

	thor::PolarVector2f m_polarVel;

	float m_deflect = 0.f;
	float m_emissionRate = 0.f;
	float m_lifeTime = 0.f;

	bool m_isRunning = false;
};

struct ColorGradient
{
	ColorGradient() = default;
	void Add(sf::Color _color, float _percentProgress);
	thor::ColorGradient gradient;
};

#endif // !PARTICLE_MANAGER_H


