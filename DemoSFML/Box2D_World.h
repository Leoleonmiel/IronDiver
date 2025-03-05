#ifndef BOX2D_WORLD_H
#define BOX2D_WORLD_H

#include <iostream>
#include "SFML/Graphics.hpp"
#include "box2D/box2D.h"
#include <functional>
#include <deque>
#include <cstdint>

class SFMLDebugDraw;
namespace b2 { class RayCastClosest; }

namespace b2
{
	class MyContactListener;

	class World
	{
	public:
		World(sf::Vector2f _gravity, sf::RenderWindow& _window);
		~World();
		//Place Before all Update functions
		void Update();
		//Place After all update functions
		void Clear();
		void Display(sf::RenderWindow& _window);
		void DisplayDebug(sf::Vector2f _camPos);

		b2World* GetWorld() { return m_world; }
		MyContactListener* GetListener() { return m_listener; }
		std::deque<b2Body*>* GetDeleteBodyList() { return &m_deleteBodyList; }
		std::deque<b2Joint*>* GetDeleteJointList() { return &m_deleteJointList; }

		bool IsCollideBetween(std::string _name1, std::string _name2);

	private:
		MyContactListener* m_listener = nullptr;

		b2World* m_world = nullptr;

		SFMLDebugDraw* m_debugDrawInstance = nullptr;

		std::deque<b2Body*> m_deleteBodyList;
		std::deque<b2Joint*> m_deleteJointList;
	};

}
#endif