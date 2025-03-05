#include "GrabHook.h"
#include "common.h"
#include "Box2D_Shape.h"
#include "Box2D_Joint.h"
#include "Box2D_World.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "TextureManager.h"

#define O {0.f, 0.f}
#define NB_FRAME 3.f
#define ANIM_RETRACT_SLOW 30.f
#define ANIM_FRAMERATE 5.f

GrabHook::GrabHook(GrabHookStats* _stats, b2::Shape* _shooterShape, GroupType _collidGroup) :
	m_stats(_stats), m_shooterShape(_shooterShape), m_collidGroup(_collidGroup)
{
	m_rayCast = new b2::RayCastClosest(GameData::World, &_stats->whiteList, &_stats->blackList);
	m_rayCast->SetNormalOffset(_stats->normalOffset);

	m_armSprite = new sf::Sprite(*TextureManager::GetTexture("Arm.png"));

	sf::Vector2u size = m_armSprite->getTexture()->getSize();
	m_armSprite->setOrigin(0, size.y / 2.f);

	m_rodeVertices = new sf::VertexArray(sf::Quads, 4);
	m_states = new sf::RenderStates;

	sf::Texture* texture = TextureManager::GetTexture("RayAnim.png");
	texture->setRepeated(true);

	sf::Vector2u raySize = texture->getSize();
	m_textureSize = { (float)raySize.x, (float)(raySize.y) / NB_FRAME };
	m_states->texture = texture;

	sf::VertexArray& quad = *m_rodeVertices;
	quad[0].texCoords = { 0.f,0.f };
	quad[1].texCoords = { 0.f,m_textureSize.y };
	quad[2].texCoords = { m_textureSize.x, m_textureSize.y };
	quad[3].texCoords = { m_textureSize.x,0.f };

	m_rayCast->SetSpriteHiter(TextureManager::CreateSprite("GrabTarget.png", true));
	m_rayCast->SetHiterColor(sf::Color::White, sf::Color::Red, sf::Color::White);
}

GrabHook::~GrabHook()
{
	Clear();
	delete m_armSprite;
	delete m_rodeSprite;
	m_rayCast->Delete();
	delete m_rodeVertices;
	delete m_states;
}

void GrabHook::Update(float _dt)
{
	InputManager& input = *InputManager::GetInstance();

	switch (m_state)
	{
	case GrabHook::AIM:
		m_angle = GetAngle(m_shooterShape->GetPosition(), GameData::GetRelativeMousePos());
		m_rayCast->Cast(m_shooterShape->GetPosition(), -m_angle, m_stats->raycastRange);
		break;
	case GrabHook::SHOOT:
		UpdateRayFoundTarget();
		break;
	case GrabHook::COLLID:
		CreateRode();
		m_state = RETRACT;
		break;
	case GrabHook::RETRACT:
		SoundManager::GetInstance()->Play("GrabRetract", true);
		UpdateRetractAccel(_dt);
		UpdateUnsplit();
		UpdateRodeAnim(_dt);
		if (input.IsLongRelease("GrabHook")) m_state = DESTROY;
		break;
	case GrabHook::SPLIT:
		break;
	case GrabHook::DESTROY:
		SoundManager::GetInstance()->Stop("GrabRetract");
		Clear();
		m_state = AIM;
		break;
	default:
		break;
	}
}

void GrabHook::UpdateRayFoundTarget()
{
	if (!m_rayCast->GetIsCast())
	{
		if (m_rayCast->GetIsHit() == b2::WHITE_LISTED)
		{
			AddTarget(m_rayCast->GetHitData());
			m_state = COLLID;
		}
		else
		{
			m_state = AIM;
		}
	}
}

void GrabHook::UpdateRetractAccel(float _dt)
{
	if (m_rode != NULL)
	{
		bool atLowerLimit = m_armRodeJoint->GetJoint()->GetJointTranslation() <= m_armRodeJoint->GetJoint()->GetLowerLimit();
		bool atUpperLimit = m_armRodeJoint->GetJoint()->GetJointTranslation() >= m_armRodeJoint->GetJoint()->GetUpperLimit();

		if (atUpperLimit)
		{
			m_rode->GetBody()->SetFixedRotation(true);
		}
		else
		{
			m_rode->GetBody()->SetFixedRotation(false);

			m_speed += m_stats->acceleration * _dt;
		}

		if (m_speed > m_stats->speedMax)
		{
			m_speed = m_stats->speedMax;
		}

		m_armRodeJoint->GetJoint()->SetMaxMotorForce(-m_speed);
	}
}

void GrabHook::UpdateUnsplit()
{
	if (m_targetList.size() > 1)
	{
		int i = (int)m_targetList.size() - 2;
		float dir = CrossProduct(m_targetList[i]->worldPos, m_targetList.back()->worldPos, m_armRodeJoint->GetWorldPos());

		if ((dir >= 0 and m_splitDir < 0) or (dir <= 0 and m_splitDir > 0))
		{
			m_targetList.back()->shape->SetIsRayHited(false);
			m_targetList.pop_back();
			m_state = COLLID;
		}
	}
}

void GrabHook::UpdateRodeAnim(float _dt)
{
	if (m_rode != NULL)
	{
		m_animTimer += _dt;
		if (m_animTimer >= 1.f / ANIM_FRAMERATE)
		{
			m_animTimer -= 1.f / ANIM_FRAMERATE;
			currFrame++;

			if (currFrame >= NB_FRAME)
			{
				currFrame = 0;
			}

			sf::VertexArray& quad = *m_rodeVertices;
			quad[0].texCoords.y = m_textureSize.y * currFrame;
			quad[1].texCoords.y = m_textureSize.y * (currFrame + 1);
			quad[2].texCoords.y = m_textureSize.y * (currFrame + 1);
			quad[3].texCoords.y = m_textureSize.y * currFrame;
		}
	}
}

void GrabHook::Display(sf::RenderWindow& _window)
{
	if (m_rode)
	{
		sf::VertexArray& quad = *m_rodeVertices;
		sf::Vector2f vec;
		sf::Vector2f vecDir;
		sf::Vector2f normal;
		float distance;

		for (int i = 0; i < m_targetList.size(); ++i)
		{
			if (i < m_targetList.size() - 1)
			{

				vec = m_targetList[i + 1]->GetDynamicWorldPos() - m_targetList[i]->GetDynamicWorldPos();
				vecDir = { -vec.y, vec.x };
				normal = Normalize(vecDir);

				quad[0].position = m_targetList[i]->GetDynamicWorldPos() + normal * m_textureSize.y / 2.f;
				quad[1].position = m_targetList[i]->GetDynamicWorldPos() - normal * m_textureSize.y / 2.f;
				quad[2].position = m_targetList[i + 1]->GetDynamicWorldPos() - normal * m_textureSize.y / 2.f;
				quad[3].position = m_targetList[i + 1]->GetDynamicWorldPos() + normal * m_textureSize.y / 2.f;
			}
			else if (i == m_targetList.size() - 1)
			{
				vec = m_armRodeJoint->GetWorldPos() - m_targetList[i]->GetDynamicWorldPos();
				vecDir = { -vec.y, vec.x };
				normal = Normalize(vecDir);

				quad[0].position = m_targetList[i]->GetDynamicWorldPos() + normal * m_textureSize.y / 2.f;
				quad[1].position = m_targetList[i]->GetDynamicWorldPos() - normal * m_textureSize.y / 2.f;
				quad[2].position = m_armRodeJoint->GetWorldPos() - normal * m_textureSize.y / 2.f;
				quad[3].position = m_armRodeJoint->GetWorldPos() + normal * m_textureSize.y / 2.f;
			}
			distance = Distance(vec);

			quad[2].texCoords.x = distance / m_textureSize.x * m_textureSize.x;
			quad[3].texCoords.x = distance / m_textureSize.x * m_textureSize.x;

			quad[0].texCoords.x -= m_speed / ANIM_RETRACT_SLOW * GameData::DeltaTime;
			quad[1].texCoords.x -= m_speed / ANIM_RETRACT_SLOW * GameData::DeltaTime;
			quad[2].texCoords.x -= m_speed / ANIM_RETRACT_SLOW * GameData::DeltaTime;
			quad[3].texCoords.x -= m_speed / ANIM_RETRACT_SLOW * GameData::DeltaTime;

			_window.draw(quad, *m_states);
		}

		m_armSprite->setPosition(m_shooterArmJoint->GetWorldPos());
		m_armSprite->setRotation(m_arm->GetAngleRad() * -180.f / _PI);
		_window.draw(*m_armSprite);
	}
	if (m_showTarget) m_rayCast->Display(_window);
}

void GrabHook::Clear()
{
	ClearRode();

	m_speed = m_stats->speedMin;

	for (int i = 0; i < m_targetList.size(); i++)
	{
		m_targetList[i]->shape->SetIsRayHited(false);
		delete m_targetList[i];
		m_targetList[i] = nullptr;
	}
	m_targetList.clear();
	m_targetList.shrink_to_fit();
}

void GrabHook::ClearRode()
{
	delete m_rode;
	m_rode = nullptr;

	delete m_armRodeJoint;
	m_armRodeJoint = nullptr;

	delete m_arm;
	m_arm = nullptr;

	delete m_shooterArmJoint;
	m_shooterArmJoint = nullptr;

	delete m_armRodeJoint;
	m_armRodeJoint = nullptr;

	delete m_rodeTargetJoint;
	m_rodeTargetJoint = nullptr;
}

void GrabHook::Shoot()
{
	m_state = SHOOT;
}

#define ARM_REDUCE 4.f
#define ARM_WIDTH 87.f
#define ARM_WR ARM_WIDTH / ARM_REDUCE

void GrabHook::CreateRode()
{
	ClearRode();
	if (m_targetList.size() > 0)
	{
		sf::Vector2f shooterPos = m_shooterShape->GetPosition();
		float distance = sqrt(DistanceSquared(shooterPos, m_targetList.back()->worldPos));
		float angle = GetAngle(shooterPos, m_targetList.back()->worldPos);

		b2Vec2 ptList[] = { O,ScreenToB2World(distance,m_stats->roadEdges),ScreenToB2World(distance,-m_stats->roadEdges) };
		m_rode = new b2::Polygone(*GameData::World, b2_dynamicBody, shooterPos, ptList, 3, "Rode", uintptr_t(this));
		m_rode->SetAngleRad(angle);
		m_rode->SetPhysics(1.f, 0.f);
		m_rode->GetBody()->SetBullet(true);
		m_rode->SetGroup(m_collidGroup);

		m_arm = new b2::Rectangle(*GameData::World, b2_dynamicBody, shooterPos + sf::Vector2f(0, 0), { 26,26 }, "Arm");
		m_arm->SetAngleRad(angle);
		m_arm->SetPhysics(1.f, 0.f);
		m_arm->SetGroup(m_collidGroup);

		m_shooterArmJoint = new b2::RevoluteJoint(*GameData::World, *m_shooterShape, *m_arm, O, O, false);
		m_armRodeJoint = new b2::PrismaticJoint(*GameData::World, *m_rode, *m_arm, O, O, false);
		m_armRodeJoint->GetJoint()->EnableMotor(true);
		m_armRodeJoint->GetJoint()->EnableLimit(true); //Trop buggué
		m_armRodeJoint->GetJoint()->SetLimits(0, distance / MtoP);
		m_speed = m_stats->speedMin;

		m_rodeTargetJoint = new b2::RevoluteJoint(*GameData::World, *m_rode, *m_targetList.back()->shape, { distance, 0 }, m_targetList.back()->localPos, true);
	}
}

void GrabHook::SplitRode(b2::Shape* _shape, b2Contact* _contact)
{
	if (m_rode != NULL)
	{
		b2WorldManifold wm;
		_contact->GetWorldManifold(&wm);

		b2Vec2 normalEnd = wm.points[0] + m_stats->normalOffset * wm.normal;
		sf::Vector2f worldPos = B2WorldToScreen(normalEnd);
		sf::Vector2f jointPos = m_arm->GetPosition();
		float radius = DistanceSquared((m_targetList.back()->worldPos + m_armRodeJoint->GetWorldPos()) / 2.f, m_armRodeJoint->GetWorldPos());
		float distance = DistanceSquared((m_targetList.back()->worldPos + m_armRodeJoint->GetWorldPos()) / 2.f, worldPos);

		if (distance <= radius)
		{
			AddTarget(_shape, wm.points[0], wm.normal);
			m_targetList.back()->shape->SetIsRayHited(true);

			int i = (int)m_targetList.size() - 2;
			m_splitDir = CrossProduct(m_targetList[i]->worldPos, m_targetList.back()->worldPos, m_armRodeJoint->GetWorldPos());

			m_state = COLLID;
		}
	}
}

void GrabHook::TearRode(b2::Shape* _shape, b2Contact* _contact)
{
	if (m_rode != NULL)
	{
		b2WorldManifold wm;
		_contact->GetWorldManifold(&wm);

		b2Vec2 normalEnd = wm.points[0] + m_stats->normalOffset * wm.normal;
		sf::Vector2f worldPos = B2WorldToScreen(normalEnd);
		float radius = DistanceSquared((m_targetList.back()->worldPos + m_armRodeJoint->GetWorldPos()) / 2.f, m_armRodeJoint->GetWorldPos());
		float distance = DistanceSquared((m_targetList.back()->worldPos + m_armRodeJoint->GetWorldPos()) / 2.f, worldPos);

		if (distance <= radius)
		{
			m_state = DESTROY;
		}
	}
}

void GrabHook::AddTarget(b2::RayCastClosest::HitData* _target)
{
	m_targetList.push_back(new b2::RayCastClosest::HitData(*_target));
}

void GrabHook::AddTarget(b2::Shape* _shape, b2Vec2 _point, b2Vec2 _normal)
{
	m_targetList.push_back(new b2::RayCastClosest::HitData(_shape, _point, _normal, m_stats->normalOffset));
}

void GrabHook::BodyGrabbedDestroyed()
{
	if (m_state == COLLID or m_state == RETRACT)
	{
		Clear();
		m_state = AIM;
	}
}

sf::Vector2f GrabHook::GetGrabHeadPos()
{
	return m_rodeTargetJoint->GetWorldPos();
}
