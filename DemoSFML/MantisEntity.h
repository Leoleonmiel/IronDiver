#ifndef MANTIS_ENTITY_H
#define MANTIS_ENTITY_H

#include "Entity.h"

class MantisEntity : public Entity
{
protected:
	Animation* m_animation = NULL;
	Animation* m_deathAnim = NULL;
	Animation* m_jumpAnim = NULL;

	bool onGround = false;
public:
	~MantisEntity() override;
	MantisEntity(sf::Vector2f _pos);
	MantisEntity(const EntityDef& _def, sf::Vector2f _pos);

	void HurtboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;

	bool GetOnGround() { return onGround; }
	void SetOnGround(bool _onGround) { onGround = _onGround; }
};

#endif // !

