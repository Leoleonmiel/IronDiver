#ifndef BOX2D_COLLISION_H
#define BOX2D_COLLISION_H

#include <iostream>
#include "SFML/Graphics.hpp"
#include "box2D/box2D.h"
#include <functional>
#include <deque>
#include <cstdint>

namespace b2
{
	struct UserData;
	class Shape;
	typedef std::function<void(UserData*)> CollisionCallback;

	struct UserData
	{
		UserData() {}
		UserData(uintptr_t _mySelf, std::string _name, b2::Shape* _shape) : ptr(_mySelf), name(_name), shape(_shape) {}
		uintptr_t ptr = NULL;
		std::string name;
		CollisionCallback* callback = NULL;
		b2::Shape* shape = NULL;
		b2Contact* contact = NULL;
	};

	struct CollidData
	{
		uintptr_t ptrFirst = NULL;
		uintptr_t ptrSecond = NULL;
		b2Fixture* fixtureFirst = NULL;
		b2Fixture* fixtureSecond = NULL;
		short nbContact = 0;
		bool isContactAtLeastOne = false;
	};

	class MyContactListener : public b2ContactListener
	{
	public:
		~MyContactListener();

		bool ReadContact(b2Contact* contact);

		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		CollidData* GetCollision(std::string _collidName) { return &m_collisions[_collidName]; }
	private:

		std::unordered_map<std::string, CollidData> m_collisions;
		std::string collidName;
	};
}
#endif

