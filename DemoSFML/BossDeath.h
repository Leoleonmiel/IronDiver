#ifndef BOSS_DEATH_H
#define BOSS_DEATH_H

#include "EntityState.h"
#include "SFML/Graphics.hpp"

class BossDeath : public EntityState
{
private:
	sf::Vector2f m_directionNormalised;

	const float m_speed = 25.f;
	bool m_transition = false;
	float m_timer = 0.f;
public:
	BossDeath(Entity* _owner, sf::Vector2f _direction);
	void InitState() override;
	void Update() override;
	void ExitState() override;
	EntityState* GetNextState() override;
};

#endif