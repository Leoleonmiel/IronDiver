#include "Box2D_Shape.h"
#include "Box2D_World.h"
#include "Box2D_Collision.h"
#include "common.h"

using namespace b2;


#pragma region Shapes

void b2::Shape::Init(World& _world, b2Shape& _shape, b2BodyType _type, sf::Vector2f _pos, std::string _name, uintptr_t _userPtr, b2::CollisionCallback* _callback)
{
	b2BodyDef bodyDef;
	bodyDef.type = _type;
	bodyDef.position.Set(_pos.x / MtoP, -_pos.y / MtoP);
	m_body = _world.GetWorld()->CreateBody(&bodyDef);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &_shape;

	fixtureDef.userData.pointer = (uintptr_t) new UserData(_userPtr, _name, this);
	((UserData*)fixtureDef.userData.pointer)->callback = _callback;

	m_body->CreateFixture(&fixtureDef);
	m_world = &_world;
	m_name = _name;
}

void b2::Shape::Display(sf::RenderWindow& _window)
{
	if (m_sprite != nullptr)
	{
		_window.draw(*m_sprite);
	}
}

b2::Shape::~Shape()
{
	m_world->GetDeleteBodyList()->push_back(m_body);

}

void b2::Shape::ApplyForce(sf::Vector2f _force)
{
	b2Vec2 force(_force.x, -_force.y);
	m_body->ApplyForce(force, m_body->GetWorldCenter(), true);
}

void b2::Shape::ApplyLinearImpulse(sf::Vector2f _impulse)
{
	b2Vec2 impulse(_impulse.x, -_impulse.y);
	m_body->ApplyLinearImpulse(impulse, m_body->GetWorldCenter(), true);
}

void b2::Shape::SetBounce(float _bounciness)
{
	b2Fixture* fixtures = m_body->GetFixtureList();

	fixtures->SetRestitution(_bounciness);

	m_body->ResetMassData();
}

void b2::Shape::SetPhysics(float _density, float _friction, bool _isSensor, float _bounce)
{
	b2Fixture* fixtures = m_body->GetFixtureList();

	fixtures->SetDensity(_density);
	fixtures->SetFriction(_friction);
	fixtures->SetRestitution(_bounce);
	fixtures->SetSensor(_isSensor);

	m_body->ResetMassData();
}

void b2::Shape::SetGroup(uint16 _groupIndex)
{
	// Parcours des fixtures attachées au corps
	for (b2Fixture* fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		// Identifier le capteur spécifique
		UserData* userData = reinterpret_cast<UserData*>(fixture->GetUserData().pointer);
		if (userData->name == m_name)
		{
			b2Filter filters = fixture->GetFilterData();
			filters.groupIndex = _groupIndex;
			fixture->SetFilterData(filters);
			break;
		}
	}
}

void b2::Shape::SetFilters(uint16 _categoryBits, uint16 _maskBits)
{
	// Parcours des fixtures attachées au corps
	for (b2Fixture* fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		// Identifier le capteur spécifique
		UserData* userData = reinterpret_cast<UserData*>(fixture->GetUserData().pointer);
		if (userData->name == m_name)
		{
			b2Filter filters = fixture->GetFilterData();
			if (_categoryBits != 0x0001) filters.categoryBits = _categoryBits;
			if (_maskBits != 0xFFFF) filters.maskBits = _maskBits;
			fixture->SetFilterData(filters);
			break;
		}
	}
}

void b2::Shape::SetPosition(sf::Vector2f _pos)
{
	_pos.x /= MtoP;
	_pos.y /= -MtoP;
	m_body->SetTransform({ _pos.x, _pos.y }, m_body->GetAngle());
}

void b2::Shape::SetName(const std::string& _name)
{
	UserData* user = reinterpret_cast<UserData*>(m_body->GetFixtureList()->GetUserData().pointer);
	user->name = _name;
	m_name = _name;
}

float b2::Shape::GetDensity()
{
	return m_body->GetFixtureList()->GetDensity();
}

sf::Vector2f b2::Shape::GetPosition()
{
	sf::Vector2f pos(m_body->GetPosition().x, m_body->GetPosition().y);
	pos.x *= MtoP;
	pos.y *= -MtoP;
	return pos;
}

sf::Vector2f b2::Shape::GetVelocity()
{
	b2Vec2 vel = m_body->GetLinearVelocity();
	//b2Vec2 vel = m_body->GetInertia();
	vel.y = -vel.y;
	return sf::Vector2f(vel.x, vel.y);
}

sf::Vector2f b2::Shape::GetLocalPosRelativeAnotherPoint(sf::Vector2f _point)
{
	return B2WorldToScreen(m_body->GetLocalPoint(ScreenToB2World(_point)));
}


bool b2::Shape::IsCollidingWith(const std::string& _name)
{
	return m_world->IsCollideBetween(m_name, _name);
}

b2::Rectangle::Rectangle(World& _world, b2BodyType _type, sf::Vector2f _pos, sf::Vector2f _size, std::string _name, uintptr_t _userPtr, CollisionCallback* _callback)
{
	b2PolygonShape polygone;
	polygone.SetAsBox((_size.x / 2) / MtoP, (_size.y / 2) / MtoP);
	m_size = _size;

	_userPtr = _userPtr == NULL ? (uintptr_t)(this) : _userPtr;
	Init(_world, polygone, _type, _pos, _name, _userPtr, _callback);
}

b2::Circle::Circle(World& _world, b2BodyType _type, sf::Vector2f _pos, float _radius, sf::Vector2f _origine, std::string _name, uintptr_t _userPtr, CollisionCallback* _callback)
{
	b2CircleShape circle;
	circle.m_radius = _radius / MtoP;
	m_radius = _radius;
	circle.m_p = b2Vec2(_origine.x / MtoP, -_origine.y / MtoP);

	_userPtr = _userPtr == NULL ? (uintptr_t)(this) : _userPtr;
	Init(_world, circle, _type, _pos, _name, _userPtr, _callback);
}

b2::Polygone::Polygone(World& _world, b2BodyType _type, sf::Vector2f _pos, b2Vec2 _pointsList[], int32 _nbPoint, std::string _name, uintptr_t _userPtr, CollisionCallback* _callback)
{
	b2PolygonShape polygone;
	polygone.Set(_pointsList, _nbPoint);

	_userPtr = _userPtr == NULL ? (uintptr_t)(this) : _userPtr;
	Init(_world, polygone, _type, _pos, _name, _userPtr, _callback);
}

b2::Edge::Edge(World& _world, b2BodyType _type, sf::Vector2f _pos, std::string _name, uintptr_t _userPtr, CollisionCallback* _callback)
{
	b2EdgeShape edge;
	edge.m_vertex1 = { _pos.x, _pos.y };
	edge.m_vertex1 = { _pos.x + 20, _pos.y + 20 };

	_userPtr = _userPtr == NULL ? (uintptr_t)(this) : _userPtr;
	Init(_world, edge, _type, _pos, _name, _userPtr, _callback);
}


b2::Point::Point(World& _world, b2BodyType _type, sf::Vector2f _pos, std::string _name, uintptr_t _userPtr, CollisionCallback* _callback)
{
	b2PolygonShape polygone;
	b2Vec2 pos[] = { {_pos.x / MtoP, -_pos.y / MtoP} };
	polygone.Set(pos, (int32)1);

	_userPtr = _userPtr == NULL ? (uintptr_t)(this) : _userPtr;
	Init(_world, polygone, _type, _pos, _name, _userPtr, _callback);
}

b2::Sensor::Sensor(Shape& _shape, sf::Vector2f _offset, sf::Vector2f _size, std::string _name, uintptr_t _userPtr, CollisionCallback* _callback)
{
	b2PolygonShape polygone;
	b2FixtureDef hitboxFixtureDef;
	b2Vec2 sensorOffset(_offset.x / MtoP, -_offset.y / MtoP); // ajuster l'offset du capteur

	m_size = _size;
	m_offset = _offset;
	m_world = _shape.GetWorld();
	m_linkedBody = _shape.GetBody();
	m_name = _name;

	polygone.SetAsBox((_size.x / 2) / MtoP, (_size.y / 2) / MtoP);

	hitboxFixtureDef.shape = &polygone;
	hitboxFixtureDef.isSensor = true;
	_userPtr = _userPtr == NULL ? (uintptr_t)(this) : _userPtr;
	hitboxFixtureDef.userData.pointer = (uintptr_t) new UserData(_userPtr, _name, &_shape);
	((UserData*)hitboxFixtureDef.userData.pointer)->callback = _callback;
	hitboxFixtureDef.density = 0;
	hitboxFixtureDef.friction = 0;
	for (int i = 0; i < polygone.m_count; ++i)
	{
		polygone.m_vertices[i] += sensorOffset;
	}

	_shape.GetBody()->CreateFixture(&hitboxFixtureDef);

}

b2::Sensor::~Sensor()
{
	b2Fixture* fixtureToRemove = nullptr;
	for (b2Fixture* fixture = m_linkedBody->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		UserData* userData = reinterpret_cast<UserData*>(fixture->GetUserData().pointer);
		if (userData->name == m_name)
		{
			fixtureToRemove = fixture;
			break;
		}
	}

	if (fixtureToRemove)
	{
		m_linkedBody->DestroyFixture(fixtureToRemove);
		m_linkedBody = nullptr;
	}
}
sf::Vector2f b2::Sensor::GetPosition()
{
	sf::Vector2f pos(m_linkedBody->GetPosition().x, m_linkedBody->GetPosition().y);
	pos.x *= MtoP;
	pos.y *= -MtoP;
	pos += m_offset;

	return pos;
}

void b2::Sensor::SetGroup(uint16 _groupIndex)
{
	// Parcours des fixtures attachées au corps
	for (b2Fixture* fixture = m_linkedBody->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		// Identifier le capteur spécifique
		UserData* userData = reinterpret_cast<UserData*>(fixture->GetUserData().pointer);
		if (userData->name == m_name)
		{
			b2Filter filters = fixture->GetFilterData();
			filters.groupIndex = _groupIndex;
			fixture->SetFilterData(filters);
			break;
		}
	}
}

void b2::Sensor::SetFilters(uint16 _categoryBits, uint16 _maskBits)
{
	// Parcours des fixtures attachées au corps
	for (b2Fixture* fixture = m_linkedBody->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		// Identifier le capteur spécifique
		UserData* userData = reinterpret_cast<UserData*>(fixture->GetUserData().pointer);
		if (userData->name == m_name)
		{
			b2Filter filters = fixture->GetFilterData();
			if (_categoryBits != 0x0001) filters.categoryBits = _categoryBits;
			if (_maskBits != 0xFFFF) filters.maskBits = _maskBits;
			fixture->SetFilterData(filters);
			break;
		}
	}
}
#pragma endregion