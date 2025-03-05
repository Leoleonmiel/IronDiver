#ifndef OCTOPUS_ENTITY_H
#define OCTOPUS_ENTITY_H

#include "Entity.h"

class OctopusEntity : public Entity
{
protected:
	Animation* m_riseAnim = nullptr;
	Animation* m_freezeFrame = nullptr;
	Animation* m_deathAnim = nullptr;
	Animation* m_beginAttackAnim = nullptr;
	Animation* m_attackLoopAnim = nullptr;

	bool m_playerHit = false;

public:
	~OctopusEntity() override;
	OctopusEntity(sf::Vector2f _pos, bool _background = false);
	OctopusEntity(const EntityDef& _def, sf::Vector2f _pos);

	void HitboxCollided(b2::UserData* _data) override;
	void HurtboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;
	void Update() override;
};

#endif