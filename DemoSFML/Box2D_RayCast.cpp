#include "Box2D_RayCast.h"
#include "Box2D_Collision.h"
#include "Box2D_World.h"
#include "Box2D_Shape.h"
#include "common.h"

using namespace b2;

RayCastManager* RayCastManager::Instance = nullptr;

RayCastManager* RayCastManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new RayCastManager();
	}
	return Instance;
}

void b2::RayCastManager::AddRayCast(RayCastClosest* _callback)
{
	m_callbacks.push_back(_callback);
}

void b2::RayCastManager::DeleteRayCast(RayCastClosest* _callback)
{
	for (size_t i = 0; i < m_callbacks.size(); i++)
	{
		if (m_callbacks[i]->m_toDelete)
		{
			delete m_callbacks[i];
			m_callbacks.erase(m_callbacks.begin() + i);
			break;
		}
	}
}

void b2::RayCastManager::StepCasts()
{
	for (RayCastClosest* callback : m_callbacks)
	{
		if (callback->m_cast)
		{
			callback->m_hit = DOESNT_HIT_YET;
			callback->m_hitData->hitShape = false;

			b2Vec2 endPos = GetPointWithAngleAndDistanceB2(callback->m_startPos, callback->m_range, callback->m_angle);
			callback->m_world->GetWorld()->RayCast(callback, callback->m_startPos, endPos);
			callback->m_cast = false;

			if (callback->m_hitData->hitShape)
			{
				if (callback->m_whiteList)
				{
					for (const auto& name : *callback->m_whiteList)
					{
						b2::Shape* shape = callback->m_hitData->shape;
						if (shape != nullptr && shape->GetName() == name)
						{
							callback->m_hit = WHITE_LISTED;
							callback->m_hitData->shape->SetIsRayHited(true);
						}
					}
				}

				if (callback->m_hit != WHITE_LISTED)
				{
					if (callback->m_blackList)
					{
						for (const auto& name : *callback->m_blackList)
						{
							b2::Shape* shape = callback->m_hitData->shape;
							if (shape != nullptr && shape->GetName() == name)
							{
								callback->m_hit = BLACK_LISTED;
							}
						}
						if (callback->m_hit != BLACK_LISTED)
						{
							callback->m_hit = IGNORED; //Not functional at the moment
						}
					}
				}
			}
			else
			{
				callback->m_hit = VOID;
				callback->ResetHit();
			}
		}
	}
}

void b2::RayCastManager::Clear()
{


	//for (auto& ray : m_callbacks)
	//{
	//	delete ray;
	//	ray = nullptr;
	//}
	//m_callbacks.clear();
	//m_callbacks.shrink_to_fit();
	//Instance = nullptr;
}

b2::RayCastClosest::RayCastClosest(World* _world, std::vector<std::string>* _whiteList, std::vector<std::string>* _ignoreList) :
	m_world(_world), m_whiteList(_whiteList), m_blackList(_ignoreList)
{
	m_hitData = new HitData();
	RayCastManager::GetInstance()->AddRayCast(this);
}

void b2::RayCastClosest::Display(sf::RenderWindow& _window)
{
	if (m_spriteHiter)
	{
		if (m_hit == WHITE_LISTED)
		{
			m_spriteHiter->setPosition(m_hitData->GetDynamicWorldPos());
			m_spriteHiter->setColor(m_whiteListedColor);
		}
		else if (m_hit == BLACK_LISTED)
		{
			m_spriteHiter->setPosition(m_hitData->GetDynamicWorldPos());
			m_spriteHiter->setColor(m_blackListedColor);
		}
		else if (m_hit == VOID)
		{
			m_spriteHiter->setPosition(GetPointWithAngleAndDistance(B2WorldToScreen(m_startPos), m_range * MtoP, -m_angle));
			m_spriteHiter->setColor(m_voidListedColor);
		}
		else if (m_hit == IGNORED)//Unfonctional
		{
			m_spriteHiter->setPosition(GetPointWithAngleAndDistance(B2WorldToScreen(m_startPos), m_range * MtoP, -m_angle));
			m_spriteHiter->setColor(sf::Color::Yellow);
		}
		_window.draw(*m_spriteHiter);
	}
}

float RayCastClosest::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
	UserData* index = reinterpret_cast<UserData*>(fixture->GetUserData().pointer);

	if (m_whiteList)
	{
		for (const auto& name : *m_whiteList)
		{
			if (index->name == name)
			{
				*m_hitData = HitData(index->shape, point, normal, m_normalOffset);

				m_hitData->hitShape = true;

				return fraction;
			}
		}
	}

	if (m_blackList)
	{
		for (const auto& name : *m_blackList)
		{
			if (index->name == name)
			{
				*m_hitData = HitData(index->shape, point, normal, m_normalOffset);
				m_hitData->hitShape = true; //Traité en sortie

				return fraction;
			}
		}
	}

	return -1.f;
}


void RayCastClosest::Cast(sf::Vector2f _startPos, float _angle, float _range)
{
	m_startPos = ScreenToB2World(_startPos);
	m_angle = _angle;
	m_range = _range / MtoP;
	m_cast = true;
}

void b2::RayCastClosest::Delete()
{
	m_toDelete = true;

	delete m_hitData;
	m_hitData = nullptr;
	delete m_spriteHiter;
	m_spriteHiter = nullptr;

	RayCastManager::GetInstance()->DeleteRayCast(this);
}

float b2::RayCastClosest::GetDynamicLength()
{
	return Distance(B2WorldToScreen(m_startPos), GetHiterPos());
}

void b2::RayCastClosest::SetNormalOffset(float _normalOffset)
{
	m_normalOffset = _normalOffset;
	if (m_hitData->shape != NULL)
	{
		m_hitData->b2WorldPos = (m_hitData->point + _normalOffset * m_hitData->normal);
		m_hitData->b2LocalPos = m_hitData->shape->GetBody()->GetLocalPoint(m_hitData->b2WorldPos);
		m_hitData->worldPos = B2WorldToScreen(m_hitData->b2WorldPos);
		m_hitData->localPos = B2WorldToScreen(m_hitData->b2LocalPos);
		m_hitData->normalOffset = _normalOffset;
	}
}

void b2::RayCastClosest::SetSpriteHiter(sf::Sprite* _sprite, float _offset)
{
	m_spriteHiter = _sprite;
	m_drawableOffset = _offset;
}

void b2::RayCastClosest::SetHiterColor(sf::Color _whiteList, sf::Color _blackList, sf::Color _void)
{
	m_whiteListedColor = _whiteList;
	m_blackListedColor = _blackList;
	m_voidListedColor = _void;
}

void b2::RayCastClosest::ResetHit()
{
	*m_hitData = HitData();
	m_startPos = { 0.f, 0.f };
	m_angle = 0.f;
	m_range = 0.f;
	m_cast = false;
}

b2::RayCastClosest::HitData::HitData(Shape* _shape, b2Vec2 _point, b2Vec2 _normal, float _normalOffset) :
	shape(_shape), point(_point), normal(_normal), normalOffset(_normalOffset)
{
	b2WorldPos = (_point + _normalOffset * _normal);
	b2LocalPos = _shape->GetBody()->GetLocalPoint(b2WorldPos);
	worldPos = B2WorldToScreen(b2WorldPos);
	localPos = B2WorldToScreen(b2LocalPos);
}

b2Vec2 b2::RayCastClosest::HitData::GetDynamicWorldPosB2()
{
	return shape->GetBody()->GetWorldPoint(b2LocalPos);
}

sf::Vector2f b2::RayCastClosest::HitData::GetDynamicWorldPos()
{
	return B2WorldToScreen(GetDynamicWorldPosB2());
}