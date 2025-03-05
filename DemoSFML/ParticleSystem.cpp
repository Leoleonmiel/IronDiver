#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(sf::Texture* _texture)
{
	m_system = new thor::ParticleSystem;
	m_system->setTexture(*_texture);
}

void ParticleSystem::AddEmitter(ParticleEmitter* _emitter)
{
	m_emittersList.push_back(_emitter);
	m_system->addEmitter(thor::refEmitter(*m_emittersList.back()->m_emitter));
}

void ParticleSystem::AddTextureRect(sf::IntRect _rect)
{
	m_system->addTextureRect(_rect);
}

void ParticleSystem::AddColorGradiant(ColorGradient& _colorGradiant)
{
	thor::ColorAnimation colorizer(_colorGradiant.gradient);
	m_system->addAffector(thor::AnimationAffector(colorizer));
}

void ParticleSystem::Update(float _dt)
{
	for (auto& data : m_emittersList)
	{
		data->m_emitter->setParticleVelocity(thor::Distributions::deflect(data->m_polarVel, data->m_deflect));
	}

	m_system->update(sf::seconds(_dt));
}

void ParticleSystem::Display(sf::RenderWindow& _window)
{
	_window.draw(*m_system);
}

void ParticleSystem::ClearAffectors()
{
	m_system->clearAffectors();
}

void ParticleSystem::AnimParticles(AnimMode _animMode, float _framerate, int _nbFrames)
{
	m_animMode = _animMode;
	m_framerate = _framerate;
	m_nbFrames = _nbFrames;

	std::function<void(thor::Particle&, float)> Animation = std::bind(&ParticleSystem::Animation, this, std::placeholders::_1, std::placeholders::_2);
	m_system->addAffector(thor::AnimationAffector(Animation));
}

void ParticleSystem::FadeParticles(float _inRatio, float _outRatio)
{
	thor::FadeAnimation fader(_inRatio, _outRatio);
	m_system->addAffector(thor::AnimationAffector(fader));
}

void ParticleSystem::TorqueParticles(float _torque)
{
	m_system->addAffector(thor::TorqueAffector(_torque));
}

void ParticleSystem::ScaleParticles(float _scaleX, float _scaleY)
{
	m_system->addAffector(thor::ScaleAffector({ _scaleX , _scaleY }));
}

void ParticleSystem::ApplyForceOnParticles(float _forceX, float _forceY)
{
	m_system->addAffector(thor::ForceAffector({ _forceX , _forceY }));
}

void ParticleSystem::Animation(thor::Particle& _particle, float _progress)
{
	int currFrame = 0;

	switch (m_animMode)
	{
	case LIFE_TIME:
	{
		float restartAtPercent = 1.f / m_nbFrames;
		currFrame = floor(_progress / restartAtPercent);
		if (currFrame >= m_nbFrames) currFrame = m_nbFrames - 1;

		break;
	}
	case ONE_SHOT:
	{
		currFrame = floor(_particle.passedLifetime.asSeconds() / m_framerate);
		if (currFrame >= m_nbFrames) currFrame = m_nbFrames - 1;
		break;
	}
	case LOOP:
	{
		currFrame = floor(_particle.passedLifetime.asSeconds() / m_framerate);
		if (currFrame >= m_nbFrames) currFrame -= m_nbFrames * floor(currFrame / m_nbFrames);
		//if (currFrame >= m_nbFrames) currFrame = 0;

		break;
	}
	default:
		break;
	}

	if (m_animMode != ANIM_MODE_UNSELECTED)
	{
		_particle.textureIndex = currFrame;
	}
}

ParticleEmitter::ParticleEmitter(float _emissionRate, float _lifeTime) :
	m_emissionRate(_emissionRate), m_lifeTime(_lifeTime)
{
	m_emitter = new thor::UniversalEmitter;

	m_emitter->setEmissionRate(0.f);
	m_emitter->setParticleLifetime(sf::seconds(_lifeTime));
}

ParticleEmitter::~ParticleEmitter()
{
	delete m_emitter;
	m_emitter = nullptr;
}

void ParticleEmitter::Start()
{
	m_isRunning = true;
	m_emitter->setEmissionRate(m_emissionRate);
}

void ParticleEmitter::Stop()
{
	m_isRunning = false;
	m_emitter->setEmissionRate(0.f);
}

void ParticleEmitter::EmitRandomFrames(int _frameMin, int _frameMax)
{
	m_emitter->setParticleTextureIndex(thor::Distributions::uniform(_frameMin, _frameMax));
}

void ParticleEmitter::SetPosition(EmitterPosFunc* _posFunc)
{
	m_emitter->setParticlePosition(*_posFunc);
}

void ParticleEmitter::SetPosition(sf::Vector2f _pos)
{
	m_emitter->setParticlePosition(_pos);
}

void ParticleEmitter::SetRandomPosition(EmitterPosFunc* _posFuncA, EmitterPosFunc* _posFuncB)
{
	//m_emitter->setParticlePosition(GetRandomPos(*(*_posFuncA), *(*_posFuncB)));
}

void ParticleEmitter::SetRandomPosition(sf::Vector2f _posA, sf::Vector2f _posB)
{
	m_emitter->setParticlePosition(GetRandomPos(_posA, _posB));
}

void ParticleEmitter::SetRotationRad(float _angleRad)
{
	m_emitter->setParticleRotation(thor::toDegree(_angleRad));
}

void ParticleEmitter::SetRotationDeg(float _angleRad)
{
	m_emitter->setParticleRotation(_angleRad);
}

void ParticleEmitter::SetRotationRandomRad(float _angleMinRad, float _angleMaxRad)
{
	m_emitter->setParticleRotation(thor::Distributions::uniform(thor::toDegree(_angleMinRad), thor::toDegree(_angleMaxRad)));
}

void ParticleEmitter::SetRotationSpeed(float _speed)
{
	m_emitter->setParticleRotationSpeed(_speed);
}

void ParticleEmitter::SetColor(sf::Color _color)
{
	m_emitter->setParticleColor(_color);
}

void ParticleEmitter::SetRandomColor(int _min, int _max)
{
	m_colorRandArea = { _min, _max };

	thor::Distribution<sf::Color> func = std::bind(&ParticleEmitter::GetRandomColor, this);
	m_emitter->setParticleColor(func);
}

void ParticleEmitter::SetStartVelocity(float _radius, float _angleDeg)
{
	m_polarVel.phi = _angleDeg;
	m_polarVel.r = _radius;
}

void ParticleEmitter::SetDeflect(float _deflect)
{
	m_deflect = _deflect;
}

void ParticleEmitter::SetEmissionRate(float _emissionRate)
{
	m_emissionRate = _emissionRate;

	if (m_isRunning)
	{
		m_emitter->setEmissionRate(_emissionRate);
	}
}

void ParticleEmitter::SetRotationRandomDeg(float _angleMinDeg, float _angleMaxDeg)
{
	m_emitter->setParticleRotation(thor::Distributions::uniform(_angleMinDeg, _angleMaxDeg));
}

sf::Vector2f ParticleEmitter::GetRandomPos(sf::Vector2f _posA, sf::Vector2f _posB)
{
	sf::Vector2f random;
	random.x = _posA.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_posB.x - _posA.x)));
	random.y = _posA.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (_posB.y - _posA.y)));
	return random;
}

sf::Color ParticleEmitter::GetRandomColor()
{
	int min = m_colorRandArea.x;
	int max = m_colorRandArea.y;

	sf::Uint8 r = min + rand() % (max - min + 1);
	sf::Uint8 g = min + rand() % (max - min + 1);
	sf::Uint8 b = min + rand() % (max - min + 1);

	return sf::Color(r, g, b);
}

void ColorGradient::Add(sf::Color _color, float _percentProgress)
{
	const float percent = _percentProgress / 100.f;
	gradient[percent] = _color;
}
