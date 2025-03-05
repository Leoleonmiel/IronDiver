#ifndef JELLYFISH_ENTITY_H
#define JELLYFISH_ENTITY_H

#include "Entity.h"

class JellyfishEntity : public Entity
{
protected:
	Animation* m_animation = NULL;
	Animation* m_freezeFrame = NULL;
	Animation* m_deathAnim = NULL;

public:
	~JellyfishEntity() override;
	JellyfishEntity(sf::Vector2f _pos);
	JellyfishEntity(const EntityDef& _def, sf::Vector2f _pos);

	void HurtboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;
};

#endif