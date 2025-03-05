#include "Box2D_World.h"
#include "Box2D_Collision.h"
#include "Box2D_RayCast.h"
#include "common.h"
#include "SFMLDebugDraw.h"
#include "CharacterController.h"

using namespace b2;

World::World(sf::Vector2f _gravity, sf::RenderWindow& _window)
{
	b2Vec2 gravity;
	gravity.Set(_gravity.x, _gravity.y);
	m_world = new b2World(gravity);

	m_debugDrawInstance = new SFMLDebugDraw(_window);
	m_debugDrawInstance->SetFlags(b2Draw::e_shapeBit | b2Draw::e_centerOfMassBit | b2Draw::e_jointBit);
	m_world->SetDebugDraw(m_debugDrawInstance);

	m_listener = new MyContactListener();
	m_world->SetContactListener(m_listener);

}

World::~World()
{
	delete m_world;
	m_world = nullptr;

	delete m_debugDrawInstance;
	m_debugDrawInstance = nullptr;

	delete m_listener;
	m_listener = nullptr;
}

void World::Update()
{
	m_world->Step((1.f / MAX_FPS) * GameData::GameSpeed, 30, 30);

	RayCastManager::GetInstance()->StepCasts();

	//m_world->RayCast(m_callback, GameData::CharaCtrl->GetPosB2(), {GameData::GetRelativeMousePos().x/ MtoP, -GameData::GetRelativeMousePos().y / MtoP});
	//b2Vec2 localPos = (m_callback->m_point + 1.f * m_callback->m_normal);
	//localPos.x *= MtoP;
	//localPos.y *= -MtoP;
	//DebugPoint gg(localPos, 5, sf::Color::Red);
}

void b2::World::Clear()
{
	for (auto& joint : m_deleteJointList)
	{
		m_world->DestroyJoint(joint);
		joint = nullptr;
	}
	m_deleteJointList.clear();
	m_deleteJointList.shrink_to_fit();

	for (auto& body : m_deleteBodyList)
	{
		UserData* userData = reinterpret_cast<UserData*>(body->GetFixtureList()->GetUserData().pointer);

		m_world->DestroyBody(body);
		body = nullptr;

		delete userData;
		userData = nullptr;
	}
	m_deleteBodyList.clear();
	m_deleteBodyList.shrink_to_fit();


}

void World::Display(sf::RenderWindow& _window)
{
}

void b2::World::DisplayDebug(sf::Vector2f _camPos)
{
	m_debugDrawInstance->cam = _camPos; //Indication de la position de la caméra au debugDraw
	m_world->DebugDraw(); //Affichage de debug
}

bool b2::World::IsCollideBetween(std::string _name1, std::string _name2)
{
	std::string nameAB = _name1 < _name2 ? (_name1 + "-" + _name2) : (_name2 + "-" + _name1);

	return m_listener->GetCollision(nameAB)->isContactAtLeastOne;
}