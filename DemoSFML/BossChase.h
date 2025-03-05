#ifndef BOSS_CHASE_H
#define BOSS_CHASE_H

#include "Entity.h"

class BossChase : public Entity
{
private:
	const float m_speed = 100.f;
	
	bool m_respawnPlayer = false;
public:
	static Entity* Boss;

	static sf::Vector2f RespawnPoint;
	static sf::Vector2f EndPoint;
	static bool BossSpawned;
	static bool BossReachedEnd;
	static bool BossBeaten;

	static sf::Vector2f PlayerCinematicSpawnBegin;
	static sf::Vector2f PlayerCinematicSpawnEnd;

	static sf::Vector2f CinematicCameraBegin;
	static sf::Vector2f CinematicCameraEnd;

	BossChase(sf::Vector2f _pos = sf::Vector2f{ 0,0 });
	~BossChase() override;
	void HitboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;

	void Update() override;
};

#endif // !BOSS_CHASE_H
