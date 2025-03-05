#ifndef PARTICLE_ENVIRONMENT_H
#define PARTICLE_ENVIRONMENT_H

#include "SFML/Graphics.hpp"
#include "ParticleSystem.h"

class ParticleEnvironment
{
public:
	enum ParticleType
	{
		DUST,
		BUTTERFLY,
		FIREFLY,

		NB_PARTICLES
	};


	ParticleEnvironment();
	~ParticleEnvironment();

	void Update(float _dt);
	void Display(sf::RenderWindow& _window);

private:

	ParticleSystem* m_systems[NB_PARTICLES] = { nullptr };
	ParticleEmitter* m_emitters[NB_PARTICLES] = { nullptr };
};



#endif