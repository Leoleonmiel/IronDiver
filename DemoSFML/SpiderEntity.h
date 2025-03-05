#ifndef SPIDER_ENTITY_H
#define SPIDER_ENTITY_H

#include "Entity.h"

class SpiderEntity : public Entity
{

public:
	SpiderEntity(sf::Vector2f _pos, bool _background = false);
	SpiderEntity(const EntityDef& _def, sf::Vector2f _pos);
	void HurtboxCollided(b2::UserData* _data) override;
	void GetLookDirection() override;
	void GenerateBaseAnimations(bool _background) override;
};

#endif // !SPIDER_H
