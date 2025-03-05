#ifndef FLY_ENTITY_H
#define FLY_ENTITY_H

#include "Entity.h"

class FlyEntity : public Entity
{
protected:
	Animation* m_animation;
	Animation* m_deathAnim;
	Animation* m_attackAnim;
	Animation* m_beginAttackAnim;
public:
	~FlyEntity() override;
	FlyEntity(sf::Vector2f _pos);
	FlyEntity(const EntityDef& _def, sf::Vector2f _pos);

	void HurtboxCollided(b2::UserData* _data) override;
	void HitboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;
};

#endif // !

