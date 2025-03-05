#pragma once
#include "Entity.h"

class AggressiveBat : public Entity
{
private:
	Animator* m_wingsAnimator = nullptr;

public:
	AggressiveBat(const EntityDef& _def, sf::Vector2f _pos);

	void GenerateBaseAnimations() override;
};