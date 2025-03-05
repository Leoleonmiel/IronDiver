#include "Boss.h"
#include "Box2D_Collision.h"
#include "Box2D_Shape.h"
#include "common.h"
#include "BossEntry.h"
#include "CharacterController.h"
#include "CharacterStats.h"
#include "EntityManager.h"
#include "Animation.h"
#include "Animator.h"
#include "TextureManager.h"
#include "ParticleManager.h"
#include "StandardParticles.h"
#include "CameraManager.h"
#include "TimeSpeedManager.h"
#include "BossDeath.h"
#include "GameState.h"
#include "FadeoutState.h"
#include "LoadingState.h"
#include "MainMenu.h"
#include "Cutscenes.h"
#include "CutsceneInstructionsInclude.h"
#include "MineState.h"

sf::Vector2f Boss::BeginPoint = { 0,0 };
sf::Vector2f Boss::BossSpawnPoint = { 0,0 };
sf::Vector2f Boss::ShockwaveSpawnPoint = { 0,0 };
bool Boss::BossSpawned = false;
bool Boss::BossAttacking = false;
bool Boss::BossBeaten = false;
sf::FloatRect Boss::BossArena = { 0,0,0,0 };

Boss::Boss(sf::Vector2f _pos)
{
	b2Filter filter;
	b2::Shape* shape;
	b2::CollisionCallback* callback;

	filter.categoryBits = F_MONSTER;
	filter.maskBits = F_PLAYER;

	m_weight = 100.f;
	m_sightRange = 1000.f;
	m_cowardly = false;
	m_flying = true;
	m_maxHealth = 1;
	m_health = m_maxHealth;
	m_position = _pos;
	m_invulnTime = 0.f;
	m_loots = new LootDef();

	m_bodyCount = 1;
	m_bodies = new b2Body * [m_bodyCount];
	m_shapes = new b2::Shape * [m_bodyCount];
	m_offsets = new sf::Vector2f[m_bodyCount];

	callback = new b2::CollisionCallback();
	*callback = std::bind(&Boss::HurtboxCollided, this, std::placeholders::_1);

	shape = new b2::Rectangle(*GameData::World, b2_kinematicBody, m_position, { 450, 250 }, "BossHurtbox", (uintptr_t)(this), callback);
	m_shapes[0] = shape;
	m_bodies[0] = shape->GetBody();
	m_offsets[0] = { -25,100 };
	shape->SetPhysics(1.0f, 0.f, false, 0.f);
	m_bodies[0]->SetGravityScale(0.f);
	m_bodies[0]->GetFixtureList()[0].SetFilterData(filter);
	m_bodies[0]->SetFixedRotation(true);

	m_state = new BossEntry(this);
}

void Boss::HitboxCollided(b2::UserData* _data)
{
}

void Boss::HurtboxCollided(b2::UserData* _data)
{
	if (!BossAttacking)
	{
		if (_data->name == "Attack" && m_invulnTime <= 0.f && m_health > 0)
		{
			CharaControl* character = (CharaControl*)_data->ptr;
			sf::Vector2f playerPos = character->GetPos();
			sf::Vector2f entityPos = GetScreenPos();
			sf::Vector2f path = entityPos - playerPos;
			float angleRad = atan2f(path.y, path.x);

			TakeDamage(character->GetStats()->GetDamage());

			if (m_health <= 0)
			{
				ChangeState(new BossDeath(this, path));
				TimeSpeedManager::SetBreakSpeed(0.1f);
				TimeSpeedManager::SetBreakTime(10.f, 0.65f);

				CameraManager::PrepareForShake(angleRad, 0.1f, 12.5f);
				CameraManager::Shake(5.f);

				sf::Sprite* sprite = new sf::Sprite();
				sprite->setOrigin(425, 425);
				sprite->setTextureRect({ 0, 0, 850, 850 });
				sprite->setRotation(angleRad * 180.f / 3.1415f);
				sprite->setScale(0.5f, 0.5f);
				sf::Texture* texture = TextureManager::GetTexture("AttackImpactFX.png");
				Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 6, 1.5f, { 0,0,850,850 }));
				GetPositionFunc* func = new GetPositionFunc;
				sf::Vector2f partPos = 0.2f * (path);
				*func = [this, partPos](void)->sf::Vector2f
					{
						sf::Vector2f entityPos = GetScreenPos();
						return entityPos - partPos;
					};
				ParticleManager::AddParticle(new StandardParticle(0.f, 0.f, 0.f, func, animation));
				CutsceneMaster::AddInstruction(new CutsceneControlFrame(&MineState::m_cinematicFrames, true, 0.5f, 100.f));
			}
			else
			{
				m_invulnTime = 1.5f;
				sf::Sprite* sprite = new sf::Sprite();
				sprite->setOrigin(425, 425);
				sprite->setTextureRect({ 0, 0, 850, 850 });
				sprite->setRotation(angleRad * 180.f / 3.1415f);
				sprite->setScale(0.5f, 0.5f);
				sf::Texture* texture = TextureManager::GetTexture("AttackImpactFX.png");
				Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 6, 12.f, { 0,0,850,850 }));
				ParticleManager::AddParticle(new StandardParticle(0.f, 0.f, 1.f, playerPos + (path * 0.8f), animation));
			}
		}
	}
	else
	{
		if (_data->name == "Chara")
		{
			GameData::CharaCtrl->AddDamage(10);
			if (GameData::CharaCtrl->GetPos().x > GetScreenPos().x)
			{
				GameData::CharaCtrl->SetKnockBack(0.f, 1500.f);
			}
			else
			{
				GameData::CharaCtrl->SetKnockBack(180.f, 1500.f);
			}
			//GameData::CharaCtrl->SetKnockBack(0.f, 500.f);
		}
	}
}

void Boss::GenerateBaseAnimations(bool _background)
{
	GetPositionFunc* posFunc;
	sf::Vector2f frameSize = { 800, 800 };
	Animation* anim;
	sf::Sprite* sprite = m_sprite;
	sf::Texture* texture = TextureManager::GetTexture("Boss_Spritesheet_Idle.png");

	posFunc = new GetPositionFunc();
	*posFunc = std::bind(&Boss::GetScreenPos, this);
	m_animators.push_back(new Animator(posFunc));

	anim = new Animation(CreateAnimation(*sprite, *texture, 24, 24.f, { 0,0,(int)frameSize.x, (int)frameSize.y }));
	m_animators[0]->AddAnimation("Idle", anim);

	texture = TextureManager::GetTexture("Boss_Spritesheet_Move.png");
	anim = new Animation(CreateAnimation(*sprite, *texture, 24, 24.f, { 0,0,(int)frameSize.x, (int)frameSize.y }));
	m_animators[0]->AddAnimation("Move", anim);

	texture = TextureManager::GetTexture("Boss_Spritesheet_Dash.png");

	anim = new Animation(CreateAnimation(*sprite, *texture, 5, 24.f, { 0,0,(int)frameSize.x, (int)frameSize.y }));
	anim->loop = false;
	m_animators[0]->AddAnimation("PrepareDash", anim);

	anim = new Animation(CreateAnimation(*sprite, *texture, 19, 24.f, { (int) (5 * frameSize.x), 0, (int)frameSize.x, (int)frameSize.y}));
	m_animators[0]->AddAnimation("Dash", anim);

	texture = TextureManager::GetTexture("Boss_Spritesheet_Onde.png");

	anim = new Animation(CreateAnimation(*sprite, *texture, 20, 16.f, { 0,0,(int)frameSize.x, (int)frameSize.y }));
	anim->loop = false;
	m_animators[0]->AddAnimation("PrepareSlam", anim);

	anim = new Animation(CreateAnimation(*sprite, *texture, 1, 1.f, { (int)(frameSize.x * 8),0,(int)frameSize.x, (int)frameSize.y}));
	m_animators[0]->AddAnimation("Death", anim);

	anim = new Animation(CreateAnimation(*sprite, *texture, 4, 24.f, { (int) (20 * frameSize.x),0,(int)frameSize.x, (int)frameSize.y }));
	anim->loop = false;
	m_animators[0]->AddAnimation("Onde", anim);

	m_animators[0]->TrySetCurrentAnimation("Idle");

	m_sprite->setOrigin({ frameSize.x / 2.f, frameSize.y / 2.f });
}

void Boss::Display()
{
	int alpha = 255;
	if ((int)(m_invulnTime / 0.3f) % 2 == 1)
	{
		alpha /= 2.f;
	}

	if (m_lookingRight)
	{
		m_sprite->setScale(-m_scale.x, m_scale.y);
	}
	else
	{
		m_sprite->setScale(m_scale.x, m_scale.y);
	}

	m_sprite->setRotation(m_rotation + m_bodies[0]->GetAngle() * 180.f / -3.14159f);
	m_sprite->setColor({ 255,255,255, (sf::Uint8) alpha });
	
	for (Animator* animator : m_animators)
	{
		animator->Display(*GameData::Window);
	}
}
