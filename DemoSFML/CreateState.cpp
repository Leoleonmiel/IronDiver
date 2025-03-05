#include "EntityStateInclude.h"
#include "Entity.h"
#include "Collectable.h"
#include "CharacterController.h"
#include "common.h"
#include "Inventory.h"
CreateState::CreateState(Entity* _owner) : EntityState(_owner)
{
}

void CreateState::InitState()
{

}

void CreateState::Update()
{
	Collectable& collectable = *(Collectable*)m_owner;

	if (collectable.GetTimeElapsed() >= 1.0f)
	{
		((Collectable*)m_owner)->AddToInventory(Inventory::playerInventory);
		collectable.SetIsInInventory(true);
		return;
	}

	collectable.SetTimeElapsed((collectable.GetTimeElapsed() + GameData::DeltaTime));


	float t = collectable.GetTimeElapsed() / 2.0f;
	collectable.SetPosition(collectable.GetPos() * (1 - t) + GameData::CharaCtrl->GetPos() * t);

}

void CreateState::ExitState()
{
	/*if (m_flyAnim != nullptr)
	{
		m_flyAnim->speed = m_speed;
	}*/

}

EntityState* CreateState::GetNextState()
{
	/*if (DistanceSquared(m_owner->GetPos(), Screen2World(GameData::CharaCtrl->GetPos())) <= (m_owner->SightRange() * m_owner->SightRange()))
	{
		if (m_owner->IsCowardly())
		{
			return new EntityRunawayFlying(m_owner);
		}
		else
		{
			return new EntityChaseFlying(m_owner);
		}
	}*/

	return nullptr;
}
