#include "Box2D_Collision.h"
#include "Box2D_Shape.h"
#include "CharacterController.h"
#include "Entity.h"
#include "GrabHook.h"
#include "common.h"
#include "MantisEntity.h"
#include "EelEntity.h"

using namespace b2;

b2::MyContactListener::~MyContactListener()
{
	//for (auto& collision : m_collisions)
	//{
	//	delete collision.second;
	//}
	//m_collisions.clear();
}

bool b2::MyContactListener::ReadContact(b2Contact* contact)
{
	if (contact->GetFixtureA()->GetUserData().pointer != NULL and
		contact->GetFixtureB()->GetUserData().pointer != NULL)
	{
		UserData* userA = reinterpret_cast<UserData*>(contact->GetFixtureA()->GetUserData().pointer);
		UserData* userB = reinterpret_cast<UserData*>(contact->GetFixtureB()->GetUserData().pointer);

		userA->contact = contact;
		userB->contact = contact;

		if (userA->name < userB->name)
		{
			collidName = (userA->name + "-" + userB->name);
			//m_collisions[collidName] = new CollidData;

			m_collisions[collidName].ptrFirst = userA->ptr;
			m_collisions[collidName].ptrSecond = userB->ptr;
			m_collisions[collidName].fixtureFirst = contact->GetFixtureA();
			m_collisions[collidName].fixtureSecond = contact->GetFixtureB();
		}
		else
		{
			collidName = (userB->name + "-" + userA->name);
			//m_collisions[collidName] = new CollidData;

			m_collisions[collidName].ptrFirst = userB->ptr;
			m_collisions[collidName].ptrSecond = userA->ptr;
			m_collisions[collidName].fixtureFirst = contact->GetFixtureB();
			m_collisions[collidName].fixtureSecond = contact->GetFixtureA();
		}

		if (userA->callback != nullptr)
		{
			(*userA->callback)(userB);
		}

		if (userB->callback != nullptr)
		{
			(*userB->callback)(userA);
		}

		return true;
	}

	return false;
}
#include <Thor/Math.hpp>
void b2::MyContactListener::BeginContact(b2Contact* contact)
{
	if (ReadContact(contact))
	{
		CollidData* colData = &m_collisions[collidName];
		colData->isContactAtLeastOne = true;
		colData->nbContact++;

		if (collidName == "CharaFeet-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);
			character->SetIsFeetColide(true);
		}
		else if (collidName == "CharaFeet-Pick")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* pick = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);
			character->SetIsFeetColide(true);
		}
		else if (collidName == "CharaLeft-Platform" or collidName == "CharaRight-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			character->SetIsSideColide(true);

		}
		else if (collidName == "CharaHead-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			character->SetIsHeadColide(true);
		}
		else if (collidName == "CharaFeet-Hurtbox")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);
			character->SetIsFeetColide(true);
		}
		else if (collidName == "Mantis_Hurtbox-Platform")
		{
			MantisEntity* mantis = reinterpret_cast<MantisEntity*>(colData->ptrFirst);
			mantis->GetBody(0)->SetLinearVelocity({ 0,0 });
			mantis->SetOnGround(true);
		}
		else if (collidName == "Eel_Hurtbox-Platform")
		{
			EelEntity* eel = reinterpret_cast<EelEntity*>(colData->ptrFirst);
			eel->GetBody(0)->SetLinearVelocity({ 0,0 });
			eel->SetIsCollided(true);
		}
		//else if (collidName == "Chara-Pick")
		//{
		//	//Decrease character life and invincible for 1s
		//	CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
		//	character->AddDamage(1);
		//}
	}
}

void b2::MyContactListener::EndContact(b2Contact* contact)
{
	if (ReadContact(contact))
	{
		CollidData* colData = &m_collisions[collidName];

		colData->nbContact--;
		colData->isContactAtLeastOne = colData->nbContact != 0;

		if (collidName == "CharaFeet-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			character->SetIsFeetColide(colData->isContactAtLeastOne);
		}
		if (collidName == "CharaFeet-Pick")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* pick = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			character->SetIsFeetColide(colData->isContactAtLeastOne);
		}
		else if (collidName == "CharaFeet-Hurtbox")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			character->SetIsFeetColide(colData->isContactAtLeastOne);
		}
		else if (collidName == "CharaLeft-Platform" or collidName == "CharaRight-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);
			character->SetIsSideColide(colData->isContactAtLeastOne);
		}
		else if (collidName == "CharaHead-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			character->SetIsHeadColide(colData->isContactAtLeastOne);
		}
		else if (collidName == "Eel_Hurtbox-Platform")
		{
			EelEntity* eel = reinterpret_cast<EelEntity*>(colData->ptrFirst);
			eel->SetIsCollided(false);
		}
		//else if (collidName == "Chara-Pick")
		//{
		//	//Decrease character life and invincible for 1s
		//	CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
		//	character->AddDamage(1);
		//}
	}
}

void b2::MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	if (ReadContact(contact))
	{
		CollidData* colData = &m_collisions[collidName];

		if (collidName == "Chara-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			contact->SetFriction(sqrt(contact->GetFixtureA()->GetFriction() * contact->GetFixtureB()->GetFriction()));
			if (character->GetIsSideColide() or character->GetIsHeadColide())
			{
				contact->SetFriction(0.0f);
			}
		}

		else if (collidName == "Chara-Pick")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);

			contact->SetFriction(sqrt(contact->GetFixtureA()->GetFriction() * contact->GetFixtureB()->GetFriction()));
			if (character->GetIsSideColide() or character->GetIsHeadColide())
			{
				contact->SetFriction(0.0f);
			}
		}

		else if (collidName == "Platform-Rode")
		{
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrFirst);
			GrabHook* hook = reinterpret_cast<GrabHook*>(colData->ptrSecond);

			contact->SetEnabled(false);

			hook->SplitRode(platform, contact);
		}
		else if (collidName == "Pick-Rode")
		{
			b2::Rectangle* pick = reinterpret_cast<b2::Rectangle*>(colData->ptrFirst);
			GrabHook* hook = reinterpret_cast<GrabHook*>(colData->ptrSecond);

			contact->SetEnabled(false);

			hook->TearRode(pick, contact);
		}
		else if (collidName == "Hurtbox-Rode")
		{
			Entity* enemy = reinterpret_cast<Entity*>(colData->ptrFirst);
			GrabHook* hook = reinterpret_cast<GrabHook*>(colData->ptrSecond);

			contact->SetEnabled(false);
		}
		else if (collidName == "Chara-Hurtbox" and m_collisions["CharaFeet-Hurtbox"].isContactAtLeastOne)
		{
			contact->SetFriction(1.f);
		}
		else if (collidName == "Chara-Hurtbox" and m_collisions["CharaLeft-Hurtbox"].isContactAtLeastOne || m_collisions["CharaRight-Hurtbox"].isContactAtLeastOne)
		{
			contact->SetFriction(0.0f);
		}
	}
}

void b2::MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	if (ReadContact(contact))
	{
		CollidData* colData = &m_collisions[collidName];

	}
}