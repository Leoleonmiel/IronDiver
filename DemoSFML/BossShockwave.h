#ifndef BOSS_SHOCKWAVE_H
#define BOSS_SHOCKWAVE_H

#include "Entity.h"


class BossShockwave : public Entity
{
private:
public:
	BossShockwave(bool _lookingRight, sf::Vector2f _pos = sf::Vector2f{ 0,0 });
	void HitboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;
	void GetLookDirection() override {};
	//void Display() override;
};

#endif // !
