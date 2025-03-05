#include "EelIdle.h"
#include "EelChase.h"
#include "EelMove.h"
#include "common.h"
#include "Entity.h"
#include "CharacterController.h"
#include "Box2D_Shape.h"
#include "EelEntity.h"

float getRotationToFaceObjeect(sf::Vector2f source, sf::Vector2f target, sf::Sprite& sprite)
{
	float dx = target.x - source.x;
	float dy = target.y - source.y;

	float value = atan2(dy, dx) * 180 / 3.1415f;
	if (value > 90 || value < -90) sprite.setScale(sprite.getScale().x, -1);
	else sprite.setScale(sprite.getScale().x, 1);
	return value;
}

EelIdle::EelIdle(Entity* _owner) : EntityState(_owner) {}

void EelIdle::InitState()
{
	EelEntity* eel = static_cast<EelEntity*>(m_owner);
	m_owner->SetCurrentAnimation("PrepareInpulsion");
	if(eel->GetScreenPos().x > eel->GetBasePos().x + m_maxCapX) eel->SetDirection(-1);
	if(eel->GetScreenPos().x < eel->GetBasePos().x + m_minCapX) eel->SetDirection(1);
	if (eel->IsCollided()) eel->SetDirection(eel->GetDirection() * -1);
	eel->SetAngle(-getRotationToFaceObjeect(m_owner->GetScreenPos(), { m_owner->GetScreenPos().x + eel->GetDirection(), m_owner->GetScreenPos().y }, *m_owner->GetSprite()) * 3.1415f / 180.f);

	eel->SetIsCollided(false);
}

void EelIdle::Update()
{
	if (!m_hitboxDead)
	{
		m_owner->GetBody(1)->SetEnabled(false);
		m_hitboxDead = true;
	}
	m_elapsedTime += GameData::DeltaTime * GameData::GameSpeed;
}

void EelIdle::ExitState() {}

EntityState* EelIdle::GetNextState()
{
	sf::Vector2f path = m_owner->GetScreenPos() - GameData::CharaCtrl->GetPos();
	path /= ZOOM;
	if (m_elapsedTime > m_duration) 
	{
		return new EelMove(m_owner);
	}
	else if (DistanceSquared(path, { 0,0 }) <= m_owner->SightRange() * m_owner->SightRange()) 
	{
		return new EelChase(m_owner);
	}
	return nullptr;
}

