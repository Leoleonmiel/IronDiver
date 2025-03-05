#include "ShrimpDeath.h"
#include "Entity.h"
#include "common.h"
#include "EntityManager.h"
#include "CharacterController.h"

ShrimpDeath::ShrimpDeath(Entity* _owner) : EntityState(_owner)
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;
}

void ShrimpDeath::InitState()
{
	float angle = -0.5f + (0.35f * m_knockbackDirection);
	sf::Vector2f direction;
	angle *= 3.14f;

	m_timer = 0;
	m_owner->FreezeBodies();

	direction = sf::Vector2f(std::cos(angle), std::sin(angle));
	direction *= m_knockbackStrength;
	m_owner->Move(direction);
	m_owner->SetGravity(1);

	m_owner->SetCurrentAnimation("Death");
	m_owner->SetFixedRotation(false);
	m_owner->GetBody(0)->SetAngularVelocity(5.f * m_knockbackDirection);
}

void ShrimpDeath::Update()
{
	m_timer += GameData::DeltaTime * GameData::GameSpeed;
	if (m_timer >= m_duration)
	{
		if (m_timer < m_duration + m_shrinkDuration)
		{
			float shrink = 1 - ((m_timer - m_duration) / m_shrinkDuration);
			m_owner->GetSprite()->setColor(sf::Color(255, 255, 255, 255 * (sf::Uint8)shrink));
			//m_owner->SetScale({ shrink, shrink });
		}
		else
		{
			EntityManager::GetInstance()->MarkForDeath(m_owner);
		}
	}
}

void ShrimpDeath::ExitState()
{

}

EntityState* ShrimpDeath::GetNextState()
{
	return nullptr;
}