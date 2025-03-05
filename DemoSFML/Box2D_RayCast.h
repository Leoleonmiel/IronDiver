#ifndef BOX2D_RAYCAST_H
#define BOX2D_RAYCAST_H

#include <iostream>
#include "SFML/Graphics.hpp"
#include "box2D/box2D.h"

namespace b2
{
	class World;
	class Shape;
	class RayCastClosest;

	enum RayCastHit
	{
		DOESNT_HIT_YET = -1,
		VOID,
		WHITE_LISTED,
		BLACK_LISTED,
		IGNORED
	};

	class RayCastManager
	{
	public:

		static RayCastManager* GetInstance();
		void AddRayCast(RayCastClosest* _callback);
		void DeleteRayCast(RayCastClosest* _callback);
		void StepCasts();
		void Clear();

		std::vector<RayCastClosest*>* GetRayCastList() { return &m_callbacks; }
	private:
		static RayCastManager* Instance;
		RayCastManager() = default;

		std::vector<RayCastClosest*> m_callbacks;
	};

	// This callback finds the closest hit. Polygon 0 is filtered.
	class RayCastClosest : public b2RayCastCallback
	{
	public:

		struct HitData
		{
			HitData() {}
			HitData(Shape* _shape, b2Vec2 _point, b2Vec2 _normal, float _normalOffset);

			b2Vec2 GetDynamicWorldPosB2();
			sf::Vector2f GetDynamicWorldPos();

			Shape* shape = NULL;
			b2Vec2 point = { 0.f, 0.f };
			b2Vec2 normal = { 0.f, 0.f };
			b2Vec2 b2LocalPos = { 0.f, 0.f };
			b2Vec2 b2WorldPos = { 0.f, 0.f };
			sf::Vector2f localPos;
			sf::Vector2f worldPos;
			float normalOffset = 0.f;
			bool hitShape = false;
		};

		RayCastClosest(World* _world, std::vector<std::string>* _whiteList = nullptr, std::vector<std::string>* _ignoreList = nullptr);

		void Display(sf::RenderWindow& _window);

		float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
		void Cast(sf::Vector2f _startPos, float _angle, float _range);
		void Delete();

		//Check if raycast hit any bodys before use it
		HitData* GetHitData() { return m_hitData; }
		float GetNormalOffset() { return m_normalOffset; }
		sf::Vector2f GetHiterPos() { return m_spriteHiter->getPosition(); }
		float GetDynamicLength();

		void SetNormalOffset(float _normalOffset);
		void SetSpriteHiter(sf::Sprite* _sprite, float _offset = 0.f);
		void SetHiterColor(sf::Color _whiteList, sf::Color _blackList = sf::Color::White, sf::Color _void = sf::Color::White);
		void SetHiterScale(float _x, float _y) { m_spriteHiter->setScale(_x, _y); }

		bool GetIsCast() { return m_cast; }
		RayCastHit GetIsHit() { return m_hit; }

		void ResetHit();

	private:
		friend class RayCastManager;

		~RayCastClosest() = default;

		World* m_world = nullptr;

		HitData* m_hitData = nullptr;

		float m_normalOffset = 0.f;

		sf::Color m_whiteListedColor;
		sf::Color m_blackListedColor;
		sf::Color m_voidListedColor;

		//Cast Data
		b2Vec2 m_startPos = { 0.f, 0.f };

		float m_angle = 0.f;
		float m_range = 0.f;
		float m_drawableOffset = 0.f;

		bool m_cast = false;
		bool m_toDelete = false;

		std::vector<std::string>* m_whiteList;
		std::vector<std::string>* m_blackList;

		sf::Sprite* m_spriteHiter = nullptr;

		RayCastHit m_hit = DOESNT_HIT_YET;
	};
}
#endif

