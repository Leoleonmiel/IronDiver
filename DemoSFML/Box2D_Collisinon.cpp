#include "Box2D_Collision.h"
#include "Box2D_Shape.h"
#include "CharacterController.h"
#include "Entity.h"
#include "GrabHook.h"
#include "common.h"
//#include "TiledManager.hpp"

using namespace b2;
//namespace b2 { class Rectangle; }

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
		else if (collidName == "CharaLeft-Platform" or collidName == "CharaRight-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);
			character->SetIsSideColide(false);
		}
		else if (collidName == "CharaHead-Platform")
		{
			CharaControl* character = reinterpret_cast<CharaControl*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			character->SetIsHeadColide(colData->isContactAtLeastOne);
		}
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

			if (m_collisions["CharaLeft-Platform"].isContactAtLeastOne or
				m_collisions["CharaRight-Platform"].isContactAtLeastOne)
			{
				contact->SetFriction(0.0f);
			}

		}

	}
}

void b2::MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	if (ReadContact(contact))
	{
		CollidData* colData = &m_collisions[collidName];

		if (collidName == "Hook-Platform")
		{
			GrabHook* hook = reinterpret_cast<GrabHook*>(colData->ptrFirst);
			b2::Rectangle* platform = reinterpret_cast<b2::Rectangle*>(colData->ptrSecond);

			b2WorldManifold wm;
			contact->GetWorldManifold(&wm);

			b2Vec2 anchor = colData->fixtureSecond->GetBody()->GetLocalPoint(wm.points[0]);
			hook->SetTargetShape(platform, sf::Vector2f(anchor.x * MtoP, anchor.y * MtoP));
		}
		else if (collidName == "Entity-Hook")
		{
			Entity* enemy = reinterpret_cast<Entity*>(colData->ptrFirst);
			GrabHook* hook = reinterpret_cast<GrabHook*>(colData->ptrSecond);

			b2WorldManifold wm;
			contact->GetWorldManifold(&wm);

			b2Vec2 anchor = colData->fixtureSecond->GetBody()->GetLocalPoint(wm.points[0]);
			hook->SetTargetShape(enemy->GetCoreShape(), sf::Vector2f(anchor.x * MtoP, anchor.y * MtoP));
		}
	}
}