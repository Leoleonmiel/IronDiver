#ifndef SPIDER_H
#define SPIDER_H

#include "Entity.h"

class Spider : public Entity
{

public:
	Spider(const EntityDef& _def, sf::Vector2f _pos = sf::Vector2f{ 0,0 });
	void HurtboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations();
};

#endif // !SPIDER_H
