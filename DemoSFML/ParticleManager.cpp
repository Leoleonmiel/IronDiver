#include "ParticleManager.h"
#include "SFML/Graphics.hpp"
#include <iostream>

std::vector<ParticleBase*> ParticleManager::m_particles;
std::vector<ParticleBase*> ParticleManager::m_markedForDeath;

int ParticleManager::m_maxParticles = 100;

void ParticleManager::UpdateParticles()
{
	ParticleBase* toDelete = nullptr;
	for (int i = (int)m_particles.size() - 1; i >= 0; i--)
	{
		ParticleBase& particle = *m_particles[i];
		particle.Update();

		if (!particle.IsAlive())
		{
			toDelete = m_particles[i];
			m_particles.erase(m_particles.begin() + i);
			delete (toDelete);
		}
	}
}

void ParticleManager::AddParticle(ParticleBase* _particle)
{
	if (m_maxParticles == -1 || m_particles.size() >= m_maxParticles)
	{
		return;
	}

	m_particles.push_back(_particle);
}

void ParticleManager::AddParticle(ParticleBase** _particle, int _count)
{
	bool full = m_maxParticles == -1 ? false : m_particles.size() >= m_maxParticles;
	while (_count > 0 && !full)
	{
		m_particles.push_back(_particle[_count - 1]);
		_count--;

		full = m_maxParticles == -1 ? false : m_particles.size() >= m_maxParticles;
	}
}

void ParticleManager::RenderParticles(sf::RenderTarget& _target)
{
	for (ParticleBase* particle : m_particles)
	{
		particle->Render(_target);
	}
}

void ParticleManager::Clear()
{
	m_particles.clear();
}