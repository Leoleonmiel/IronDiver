#ifndef SHRIMP_ENTITY_H
#define SHRIMP_ENTITY_H

#include "Entity.h"

class ShrimpEntity : public Entity
{
public:
	ShrimpEntity(sf::Vector2f _pos);
	ShrimpEntity(const EntityDef& _def, sf::Vector2f _pos);

	void HurtboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;
};

#endif