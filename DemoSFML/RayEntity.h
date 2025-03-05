#ifndef RAY_ENTITY_H
#define RAY_ENTITY_H

#include "Entity.h"

class RayEntity : public Entity
{
public:
	RayEntity(sf::Vector2f _pos);
	RayEntity(const EntityDef& _def, sf::Vector2f _pos);

	void HurtboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;
	void GetLookDirection() override;
};

#endif // !
