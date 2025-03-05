#include "SpiderDeath.h"
#include "Entity.h"
#include "common.h"
#include "EntityManager.h"
#include "CharacterController.h"

SpiderDeath::SpiderDeath(Entity* _owner) : EntityState(_owner)
{
	float ownerPos = m_owner->GetPos().x;
	float targetPos = GameData::CharaCtrl->GetPos().x / MtoP;
	m_knockbackDirection = ownerPos > targetPos ? 1.f : -1.f;
}

void SpiderDeath::InitState()
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
}

void SpiderDeath::Update()
{
	m_timer += GameData::DeltaTime * GameData::GameSpeed;
	if (m_timer >= m_duration)
	{
		EntityManager::GetInstance()->MarkForDeath(m_owner);
	}
}

void SpiderDeath::ExitState()
{

}

EntityState* SpiderDeath::GetNextState()
{
	return nullptr;
}