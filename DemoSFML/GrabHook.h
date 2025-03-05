#ifndef GRAB_HOOK
#define GRAB_HOOK

#include <iostream>
#include <deque>
#include <vector>
#include "SFML/Graphics.hpp"
#include "box2D/box2D.h"
#include "Box2D_RayCast.h"

namespace b2 { class Circle; }
namespace b2 { class Rectangle; }
namespace b2 { class Polygone; }
namespace b2 { class Shape; }
namespace b2 { class DistanceJoint; }
namespace b2 { class WeldJoint; }
namespace b2 { class PrismaticJoint; }
namespace b2 { class RevoluteJoint; }
namespace b2 { class DistanceJoin; }

enum GroupType;

struct GrabHookStats
{
	float normalOffset = 0.f;
	float roadEdges = 0.f;
	float cooldown = 0.f;
	float acceleration = 0.f;
	float raycastRange = 0.f;
	float speedMin = 0.f;
	float speedMax = 0.f;

	std::vector<std::string> whiteList;
	std::vector<std::string> blackList;
};

class GrabHook
{
public:
	enum State
	{
		AIM,
		SHOOT,
		COLLID,
		RETRACT,
		SPLIT,
		DESTROY
	};

	GrabHook(GrabHookStats* _stats, b2::Shape* _shooterShape, GroupType _collidGroup);
	~GrabHook();

	void Update(float _dt);
	void UpdateRayFoundTarget();
	void UpdateRetractAccel(float _dt);
	void UpdateUnsplit();
	void UpdateRodeAnim(float _dt);

	void Display(sf::RenderWindow& _window);

	void Clear();
	void ClearRode();

	//void Shoot(float _angleRad);
	void Shoot();
	void CreateRode();
	void SplitRode(b2::Shape* _shape, b2Contact* _contact);
	void TearRode(b2::Shape* _shape, b2Contact* _contact);

	void AddTarget(b2::RayCastClosest::HitData* _target);
	void AddTarget(b2::Shape* _shap, b2Vec2 _point, b2Vec2 _normal);

	void BodyGrabbedDestroyed();

	b2::RevoluteJoint* GetRodeTargetJoint() { return m_rodeTargetJoint; }
	State GetState() { return m_state; }
	void SetState(State _state) { m_state = _state; }
	void SetShowTarget(bool _show) { m_showTarget = _show; }
	b2::Rectangle* GetArm() { return m_arm; }
	b2::Shape* GetShooterShape() { return m_shooterShape; }
	//void AddTargetData(b2::Shape* _shape = nullptr, b2Contact *_contact = nullptr);
	//< 0 : Right; > 0 : Left; = 0 : Colinear
	inline float CrossProduct(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) { return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x); }
	b2::RayCastClosest* GetRayCast() { return m_rayCast; }
	std::deque<b2::RayCastClosest::HitData*>* GetTargetList() { return &m_targetList; }	
	sf::Vector2f GetGrabHeadPos();
private:
	sf::Vector2f m_targetDynamiquePos;
	sf::Vector2f m_textureSize;

	std::deque<b2::RayCastClosest::HitData*> m_targetList;

	float m_angle = 0.f;
	float m_speed = 0.f;
	float m_splitDir = 0.f;
	float m_animTimer = 0.f;

	int currFrame = 0;

	bool m_arriverADestination = false;
	bool m_showTarget = true;

	GroupType m_collidGroup = (GroupType) 0;

	b2::Shape* m_shooterShape = nullptr;

	b2::Polygone* m_rode = nullptr;
	b2::Rectangle* m_arm = nullptr;

	b2::RevoluteJoint* m_shooterArmJoint = nullptr;
	b2::PrismaticJoint* m_armRodeJoint = nullptr;
	b2::RevoluteJoint* m_rodeTargetJoint = nullptr;

	b2::RayCastClosest* m_rayCast = nullptr;

	State m_state = AIM;

	GrabHookStats* m_stats = nullptr;

	sf::Sprite* m_armSprite = nullptr;
	sf::Sprite* m_rodeSprite = nullptr;

	sf::RenderStates* m_states = NULL;

	//std::vector<sf::VertexArray*> m_rodeVertices;
	sf::VertexArray* m_rodeVertices;
};

#endif
