#ifndef PARTICLE_MANAGER_H
#define PARTICLE_MANAGER_H

#include <vector>
#include "SFML/Graphics.hpp"

class ParticleBase
{
public:
	virtual void Update() = 0;
	virtual bool IsAlive() = 0;
	virtual void Render(sf::RenderTarget& _target) = 0;
};

class ParticleManager
{
public:

	static void UpdateParticles();
	static void AddParticle(ParticleBase* _particle);
	static void AddParticle(ParticleBase** _particles, int _count);
	static void RenderParticles(sf::RenderTarget& _target);

	static void Clear();

private:
	static std::vector<ParticleBase*> m_particles;
	static std::vector<ParticleBase*> m_markedForDeath;

	static int m_maxParticles;
};

#endif