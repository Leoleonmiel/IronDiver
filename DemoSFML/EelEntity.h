#ifndef EEL_ENTITY_H
#define EEL_ENTITY_H

#include "Entity.h"

class EelEntity : public Entity
{
protected:
	bool isCollided = false;
	float m_direction = 0.f;
	sf::Vector2f basePos;
	float m_angle = 0.f;
public:
	~EelEntity() override;
	EelEntity(sf::Vector2f _pos);
	EelEntity(const EntityDef& _def, sf::Vector2f _pos);

	void HurtboxCollided(b2::UserData* _data) override;
	void GenerateBaseAnimations(bool _background) override;
	void GetLookDirection() override;
	void Display() override;
	void Update() override;

	bool IsCollided() { return isCollided; }
	void SetIsCollided(bool _isCollided) { isCollided = _isCollided; }

	float GetDirection() { return m_direction; }
	void SetDirection(float _direction) { m_direction = _direction; }

	sf::Vector2f GetBasePos() { return basePos; }
	void SetBasePos(sf::Vector2f _pos) { basePos = _pos; }

	float GetAngle() { return m_angle; }
	void SetAngle(float _angle) { m_angle = _angle; }
};

#endif // !

