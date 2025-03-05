#ifndef BOX2D_SHAPE_H
#define BOX2D_SHAPE_H

#include <iostream>
#include "SFML/Graphics.hpp"
#include "box2D/box2D.h"
#include <functional>
#include <deque>
#include <cstdint>



namespace b2
{
	struct UserData;
	typedef std::function<void(UserData*)> CollisionCallback;
	class World;

	class Shape
	{
	public:
		Shape() {}
		~Shape();
		void Init(World& _world, b2Shape& _shape, b2BodyType _type, sf::Vector2f _pos, std::string _name, uintptr_t _userPtr = NULL, CollisionCallback* _callback = NULL);
		void Display(sf::RenderWindow& _window);

		void ApplyForce(sf::Vector2f _force);
		void ApplyLinearImpulse(sf::Vector2f _impulse);

		void SetBounce(float _bounciness);
		void SetPhysics(float _density = 1.f, float _friction = 0.3f, bool _isSensor = false, float _bounce = 0);
		void SetGroup(uint16 _groupIndex = 0);
		void SetFilters(uint16 _categoryBits = 0x0001, uint16 _maskBits = 0xFFFF);
		void SetPosition(sf::Vector2f _pos);
		void SetAngleRad(float _angleRad) { m_body->SetTransform(m_body->GetPosition(), -_angleRad); }
		void SetVelocityX(float _velX) { m_body->SetLinearVelocity({ _velX, m_body->GetLinearVelocity().y }); }
		void SetVelocityY(float _velY) { m_body->SetLinearVelocity({ m_body->GetLinearVelocity().x, _velY }); }
		void SetSprite(sf::Sprite& _sprite) { m_sprite = &_sprite; }
		void SetIsRayHited(bool _rayHited) { m_rayHited = _rayHited; }
		void SetName(const std::string& _name);

		float GetDensity();

		sf::Vector2f GetPosition();
		float GetAngleRad() { return m_body->GetAngle(); }
		b2Body* GetBody() { return m_body; }
		World* GetWorld() { return m_world; }
		std::string GetName() { return m_name; }
		sf::Vector2f GetVelocity();
		sf::Vector2f GetLocalPosRelativeAnotherPoint(sf::Vector2f _point);
		bool GetGrabbed() { return m_rayHited; }

		bool IsCollidingWith(const std::string& _name);

	protected:

		World* m_world = nullptr;
		b2Body* m_body = nullptr;
		sf::Sprite* m_sprite = nullptr;
		std::string m_name;

		bool m_rayHited = false;
	private:
	};

	class Rectangle : public Shape
	{
	public:
		Rectangle(World& _world, b2BodyType _type, sf::Vector2f _pos, sf::Vector2f _size, std::string _name, uintptr_t _userPtr = 0, CollisionCallback* _callback = NULL);
		sf::Vector2f GetSize() { return m_size; }
	private:
		sf::Vector2f m_size;
	};

	class Circle : public Shape
	{
	public:
		Circle(World& _world, b2BodyType _type, sf::Vector2f _pos, float _radius, sf::Vector2f _origine, std::string _name, uintptr_t _userPtr = 0, CollisionCallback* _callback = NULL);

	private:

		float m_radius = 0;
	};

	class Polygone : public Shape
	{
	public:
		Polygone(World& _world, b2BodyType _type, sf::Vector2f _pos, b2Vec2 _pointsList[], int32 _nbPoint, std::string _name, uintptr_t _userPtr = 0, CollisionCallback* _callback = NULL);

	private:

	};

	class Edge : public Shape
	{
	public:
		Edge(World& _world, b2BodyType _type, sf::Vector2f _pos, std::string _name, uintptr_t _userPtr = 0, CollisionCallback* _callback = NULL);

	private:

	};

	class Point : public Shape
	{
	public:
		Point(World& _world, b2BodyType _type, sf::Vector2f _pos, std::string _name, uintptr_t _userPtr = 0, CollisionCallback* _callback = NULL);

	private:

	};

	class Sensor
	{
	public:
		Sensor(Shape& _shape, sf::Vector2f _offset, sf::Vector2f _size, std::string _name, uintptr_t _ptr = 0, CollisionCallback* _callback = NULL);
		~Sensor();

		sf::Vector2f GetPosition();

		void SetGroup(uint16 _groupIndex = 0);
		void SetFilters(uint16 _categoryBits = 0x0001, uint16 _maskBits = 0xFFFF);

	private:
		World* m_world = nullptr;
		b2Body* m_linkedBody = nullptr;

		sf::Vector2f m_size;
		sf::Vector2f m_offset;
		std::string m_name;
	};
}
#endif // !BOX2D_SHAPE_H
