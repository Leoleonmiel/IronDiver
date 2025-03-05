#include "EelChase.h"
#include "Entity.h"
#include "common.h"
#include "EelIdle.h"
#include "EelAttack.h"
#include "Box2D_Shape.h"
#include "CharacterController.h"
#include "EelEntity.h"

float getRotationToFaceObject(sf::Vector2f source, sf::Vector2f target, sf::Sprite& sprite)
{
	float dx = target.x - source.x;
	float dy = target.y - source.y;

	float value = atan2(dy, dx) * 180 / 3.1415f;
	if(value > 90 || value < -90) sprite.setScale(sprite.getScale().x , -1);
	else sprite.setScale(sprite.getScale().x, 1);
	return value;
}

EelChase::EelChase(Entity* _owner) : EntityState(_owner)
{
	if (!m_hitboxDead)
	{
		m_owner->GetBody(1)->SetEnabled(false);
		m_hitboxDead = true;
	}
	
}

void EelChase::InitState()
{
	m_timer = 0.f;
	m_owner->SetCurrentAnimation("Attack");
}

void EelChase::Update()
{
	m_timer += GameData::DeltaTime * GameData::GameSpeed;
	sf::Vector2f path = m_owner->GetScreenPos() - (GameData::CharaCtrl->GetPos() - sf::Vector2f{ 0, 0 });
	path *= -1.f;
	path /= std::sqrt(DistanceSquared(path, { 0,0 }));
	path /= ZOOM;

	path *= m_speed * ZOOM;
	m_owner->Move(path);
	EelEntity* eel = static_cast<EelEntity*>(m_owner);
	eel->SetAngle(-getRotationToFaceObject(m_owner->GetScreenPos(), GameData::CharaCtrl->GetPos(), *m_owner->GetSprite()) * 3.1415f / 180.f);
}

void EelChase::ExitState()
{
}

EntityState* EelChase::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (sqrt(DistanceSquared(path, { 0,0 })) <= m_attackRange)
	{
		return new EelAttack(m_owner);
	}
	else if (DistanceSquared(path, { 0,0 }) > m_owner->SightRange() * m_owner->SightRange() && m_timer > m_duration)
	{
		m_owner->GetBody(0)->SetLinearVelocity({0,0});
		return new EelIdle(m_owner);
	}
	return nullptr;
}
