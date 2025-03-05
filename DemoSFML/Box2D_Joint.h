#ifndef BOX2D_JOINT_H
#define BOX2D_JOINT_H

#include <iostream>
#include "SFML/Graphics.hpp"
#include "box2D/box2D.h"
#include <functional>
#include <deque>
#include <cstdint>

namespace b2
{
	class World;
	class Shape;

	class Joint
	{
	public:
		Joint() {}
		//~Joint();

		World* GetWorld() { return m_world; }


	protected:
		World* m_world = nullptr;
		std::string m_name;

	private:
	};

	class DistanceJoint : public Joint
	{
	public:
		DistanceJoint(World& _world, b2::Shape& _shapeA, b2::Shape& _shapeB, sf::Vector2f _offsetA, sf::Vector2f _offsetB, bool _isCollide);
		~DistanceJoint();

		b2DistanceJoint* GetJoint() { return m_joint; }
		b2Vec2 GetB2WorldPos() { return m_joint->GetBodyA()->GetWorldPoint(m_joint->GetLocalAnchorA()); }
		sf::Vector2f GetWorldPos();
	private:
		b2DistanceJoint* m_joint = nullptr;
	};

	class PrismaticJoint : public Joint
	{
	public:
		PrismaticJoint(World& _world, b2::Shape& _rail, b2::Shape& _slider, sf::Vector2f _offsetA, sf::Vector2f _offsetB, bool _isCollide);
		~PrismaticJoint();
		b2PrismaticJoint* GetJoint() { return m_joint; }
		b2Vec2 GetB2WorldPos() { return m_joint->GetBodyB()->GetWorldPoint(m_joint->GetLocalAnchorB()); }
		sf::Vector2f GetWorldPos();
	private:
		b2PrismaticJoint* m_joint = nullptr;
	};

	class RevoluteJoint : public Joint
	{
	public:
		RevoluteJoint(World& _world, b2::Shape& _shapeA, b2::Shape& _shapeB, sf::Vector2f _offsetA, sf::Vector2f _offsetB, bool _isCollide);
		~RevoluteJoint();
		b2RevoluteJoint* GetJoint() { return m_joint; }
		b2Vec2 GetB2WorldPos() { return m_joint->GetBodyA()->GetWorldPoint(m_joint->GetLocalAnchorA()); }

		sf::Vector2f GetWorldPos();
	private:
		b2RevoluteJoint* m_joint = nullptr;
	};

	class WeldJoint : public Joint
	{
	public:
		WeldJoint(World& _world, b2::Shape& _shapeA, b2::Shape& _shapeB, sf::Vector2f _offsetA, sf::Vector2f _offsetB, bool _isCollide);
		~WeldJoint();
		b2WeldJoint* GetJoint() { return m_joint; }
		b2Vec2 GetB2WorldPos() { return m_joint->GetBodyA()->GetWorldPoint(m_joint->GetLocalAnchorA()); }
		sf::Vector2f GetWorldPos();
	private:
		b2WeldJoint* m_joint = nullptr;

	};
}
#endif
