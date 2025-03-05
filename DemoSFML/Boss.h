#ifndef BOSS_H
#define BOSS_H

#include "Entity.h"

class Boss : public Entity
{
public:
	static sf::Vector2f BeginPoint;
	static sf::Vector2f BossSpawnPoint;
	static sf::Vector2f ShockwaveSpawnPoint;
	static bool BossSpawned;
	static bool BossAttacking;
	static bool BossBeaten;
	static sf::FloatRect BossArena;

	Boss(sf::Vector2f _pos = sf::Vector2f{ 0,0 });
	void HitboxCollided(b2::UserData* _data) override;
	void HurtboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background = false) override;
	void Display() override;
};

#endif // !BOSS_H
