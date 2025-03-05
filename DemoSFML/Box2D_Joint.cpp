#include "Box2D_Joint.h"
#include "Box2D_Shape.h"
#include "Box2D_World.h"
#include "common.h"


using namespace b2;

b2::DistanceJoint::DistanceJoint(World& _world, b2::Shape& _shapeA, b2::Shape& _shapeB, sf::Vector2f _offsetA, sf::Vector2f _offsetB, bool _isCollide)
{
	b2DistanceJointDef jointDef;

	jointDef.localAnchorA = ScreenToB2World(_offsetA);
	jointDef.localAnchorB = ScreenToB2World(_offsetB);
	jointDef.bodyA = _shapeA.GetBody();
	jointDef.bodyB = _shapeB.GetBody();
	jointDef.collideConnected = _isCollide;

	jointDef.minLength = 0.f;
	jointDef.maxLength = 0.f;
	jointDef.length = 0.f;
	jointDef.damping = 0.f;
	jointDef.stiffness = 0.f;
	jointDef.userData.pointer = (uintptr_t)"Distance";

	m_joint = (b2DistanceJoint*)_world.GetWorld()->CreateJoint(&jointDef);

	m_world = &_world;
}
b2::DistanceJoint::~DistanceJoint()
{
	m_world->GetDeleteJointList()->push_back(m_joint);
}

sf::Vector2f b2::DistanceJoint::GetWorldPos()
{
	return B2WorldToScreen(GetB2WorldPos());
}


b2::PrismaticJoint::PrismaticJoint(World& _world, b2::Shape& _rail, b2::Shape& _slider, sf::Vector2f _offsetA, sf::Vector2f _offsetB, bool _isCollide)
{
	b2PrismaticJointDef jointDef;

	jointDef.localAnchorA = ScreenToB2World(_offsetA);
	jointDef.localAnchorB = ScreenToB2World(_offsetB);
	jointDef.bodyA = _rail.GetBody();
	jointDef.bodyB = _slider.GetBody();
	jointDef.collideConnected = _isCollide;
	jointDef.userData.pointer = (uintptr_t)"Prismatic";

	m_joint = (b2PrismaticJoint*)_world.GetWorld()->CreateJoint(&jointDef);

	m_world = &_world;
}
b2::PrismaticJoint::~PrismaticJoint()
{
	m_world->GetDeleteJointList()->push_back(m_joint);
}

sf::Vector2f b2::PrismaticJoint::GetWorldPos()
{
	return B2WorldToScreen(GetB2WorldPos());
}

b2::RevoluteJoint::RevoluteJoint(World& _world, b2::Shape& _shapeA, b2::Shape& _shapeB, sf::Vector2f _offsetA, sf::Vector2f _offsetB, bool _isCollide)
{
	b2RevoluteJointDef jointDef;

	jointDef.localAnchorA = ScreenToB2World(_offsetA);
	jointDef.localAnchorB = ScreenToB2World(_offsetB);
	jointDef.bodyA = _shapeA.GetBody();
	jointDef.bodyB = _shapeB.GetBody();
	jointDef.collideConnected = _isCollide;

	jointDef.userData.pointer = (uintptr_t)"Revolute";

	m_joint = (b2RevoluteJoint*)_world.GetWorld()->CreateJoint(&jointDef);
	m_world = &_world;
}
b2::RevoluteJoint::~RevoluteJoint()
{
	m_world->GetDeleteJointList()->push_back(m_joint);
}

sf::Vector2f b2::RevoluteJoint::GetWorldPos()
{
	return B2WorldToScreen(GetB2WorldPos());
}

b2::WeldJoint::WeldJoint(World& _world, b2::Shape& _shapeA, b2::Shape& _shapeB, sf::Vector2f _offsetA, sf::Vector2f _offsetB, bool _isCollide)
{
	b2WeldJointDef jointDef;

	jointDef.localAnchorA = ScreenToB2World(_offsetA);
	jointDef.localAnchorB = ScreenToB2World(_offsetB);
	jointDef.bodyA = _shapeA.GetBody();
	jointDef.bodyB = _shapeB.GetBody();
	jointDef.collideConnected = _isCollide;
	jointDef.userData.pointer = (uintptr_t)"Weld";

	m_joint = (b2WeldJoint*)_world.GetWorld()->CreateJoint(&jointDef);

	m_world = &_world;
}
b2::WeldJoint::~WeldJoint()
{
	m_world->GetDeleteJointList()->push_back(m_joint);
}

sf::Vector2f b2::WeldJoint::GetWorldPos()
{
	return B2WorldToScreen(GetB2WorldPos());
}
