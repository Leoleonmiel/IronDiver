#include "CharacterController.h"

#include <box2d/box2d.h>
#include "Box2D_Shape.h"
#include "Box2D_Joint.h"
#include "Box2D_Collision.h"
#include "Box2D_World.h"
#include "Box2D_RayCast.h"

#include <iostream>

#include "common.h"
#include "InputManager.h"
#include "GrabHook.h"
#include "Inventory.h"
#include "TextureManager.h"
#include "CharacterStats.h"
#include "Animation.h"
#include "Animator.h"
#include "SoundManager.h"

#include "LoadingState.h"
#include "GameOverState.h"
#include "GameState.h"
#include "FadeOutState.h"

#include "CameraManager.h"
#include "TimeSpeedManager.h"
#include "WorldManager.hpp"
#include "MineState.h"
#include "ParticleSystem.h"
#include "ParticleManager.h"
#include "StandardParticles.h"


using namespace sf;

#define SPRITE_SCALE Vector2f(1.f, 1.f)
#define INVULNERABILITY_TIME 2.f
#define BLINK_SPEED 8.f
#define BLINK_COLOR sf::Color(150, 0, 0, 150)
#define FS 200 //Frame Size

#define WorldManager WorldManager::GetInstance()

CharaControl::CharaControl(CharacterStats* _stats) : m_stats(_stats)
{
	sf::Vector2f pos = { SCREEN_SIZE.x / 2, SCREEN_SIZE.y / 2 };
	m_hitbox = new b2::Rectangle(*GameData::World, b2_dynamicBody, pos, _stats->m_coreSize, "Chara", (uintptr_t)this);
	m_hitbox->SetPhysics(_stats->m_density, _stats->m_friction);
	m_hitbox->GetBody()->SetFixedRotation(true);

	m_hitbox->SetGroup(G_PLAYER);
	m_sensor[S_HEAD] = new b2::Sensor(*m_hitbox, { 0, -_stats->m_coreSize.y / 2 - _stats->m_headSize.y / 2 }, _stats->m_headSize, "CharaHead", (uintptr_t)this);
	m_sensor[S_FEET] = new b2::Sensor(*m_hitbox, { 0, _stats->m_coreSize.y / 2 + _stats->m_feetSize.y / 2 }, _stats->m_feetSize, "CharaFeet", (uintptr_t)this);
	m_sensor[S_WALL_L] = new b2::Sensor(*m_hitbox, { -_stats->m_coreSize.x / 2 - _stats->m_wallSensorSize.x / 2, 0 }, _stats->m_wallSensorSize, "CharaLeft", (uintptr_t)this);
	m_sensor[S_WALL_R] = new b2::Sensor(*m_hitbox, { _stats->m_coreSize.x / 2 + _stats->m_wallSensorSize.x / 2, 0 }, _stats->m_wallSensorSize, "CharaRight", (uintptr_t)this);

	for (int i = 0; i < NB_SENSOR; i++)
	{
		if (i != S_ATTACK)
		{
			m_sensor[i]->SetGroup(G_PLAYER);
		}
	}

	m_timerAttackCooldown = m_stats->m_attackCooldown;
	m_hitboxAttackSideOffset = { _stats->m_coreSize.x / 2 + _stats->m_attackSize.x / 2, 0 };
	m_hitboxAttackUpOffset = { 0, -_stats->m_coreSize.y / 2 - _stats->m_attackSize.y / 2 };
	m_hitboxAttackDownOffset = { 0, _stats->m_coreSize.y / 2 + _stats->m_attackSize.y / 2 };

	m_direction = { RIGHT, NONE };
	m_numDir = { 1, 0 };

	m_grabHook = new GrabHook(_stats->m_grabHookStats, m_hitbox, G_PLAYER);

	LoadAnimations();

	m_rayCastShadow = new b2::RayCastClosest(GameData::World, &_stats->m_shadowWhiteList);
	m_rayCastShadow->SetSpriteHiter(TextureManager::CreateSprite("Shadow.png", true));

	//////////////////////////////////////////////////////////////////////

	m_grassSystem = new ParticleSystem(TextureManager::GetTexture("GrassFX.png"));
	for (size_t i = 0; i < 8; i++)
	{
		m_grassSystem->AddTextureRect(sf::IntRect(i * 200, 0, 200, 85));
	}

	m_grassEmitter = new ParticleEmitter(8.f, 1.f);
	m_grassEmitter->EmitRandomFrames(0, 7);
	m_grassEmitter->SetRotationRandomDeg(0.f, 360.f);
	m_grassEmitter->SetRotationSpeed(100.f);

	EmitterPosFunc* func = new EmitterPosFunc;
	*func = std::bind(&CharaControl::GetFootPos, this);
	m_grassEmitter->SetPosition(func);

	m_grassSystem->AddEmitter(m_grassEmitter);
	m_grassSystem->FadeParticles(0.f, 0.15f);

	////////////////////////////////////////////////////////////////////////

	m_dirtSystem = new ParticleSystem(TextureManager::GetTexture("DirtFX.png"));
	for (size_t i = 0; i < 4; i++)
	{
		m_dirtSystem->AddTextureRect(sf::IntRect(i * 200, 0, 200, 63));
	}

	m_dirtEmitter = new ParticleEmitter(1.f, 0.5f);
	m_dirtEmitter->SetPosition(func);

	m_dirtSystem->AddEmitter(m_dirtEmitter);
	m_dirtSystem->AnimParticles(ParticleSystem::LIFE_TIME, 0.f, 4);
}

CharaControl::~CharaControl()
{
	delete m_hitbox;
	delete m_grabHook;
	m_rayCastShadow->Delete();

	for (auto& sensor : m_sensor)
	{
		delete sensor;
	}
}

void CharaControl::LoadAnimations()
{
	GetPositionFunc* func = new GetPositionFunc;
	*func = std::bind(&CharaControl::GetFootPos, this);
	m_animator = new Animator(func);

	sf::Texture playerSpritsheet = *TextureManager::GetTexture("Player_Spritesheet.png");
	m_sprite = new sf::Sprite(playerSpritsheet);

	m_beginMoveAnimation = new Animation(CreateAnimation(*m_sprite, playerSpritsheet, 4, 24.f, { 0,0,FS,FS }));
	m_beginMoveAnimation->loop = false;
	m_moveAnimation = new Animation(CreateAnimation(*m_sprite, playerSpritsheet, 2, 12.f, { 0,FS,FS,FS }));
	m_endMoveAnimation = new Animation(CreateAnimation(*m_sprite, playerSpritsheet, 7, 24.f, { 0,FS * 2,FS,FS }));
	m_endMoveAnimation->loop = false;
	m_idleAnimation = new Animation(CreateAnimation(*m_sprite, playerSpritsheet, 2, 12.f, { 0,FS * 3,FS,FS }));
	m_jumpAnimation = new Animation(CreateAnimation(*m_sprite, playerSpritsheet, 3, 8.f, { 0,FS * 4,FS,FS }));
	m_jumpAnimation->loop = false;
	m_fallContactAnimation = new Animation(CreateAnimation(*m_sprite, playerSpritsheet, 2, 8.f, { FS * 4,FS * 4,FS,FS }));
	m_fallContactAnimation->loop = false;
	m_fallAnimation = new Animation(CreateAnimation(*m_sprite, playerSpritsheet, 1, 8.f, { FS * 3,FS * 4,FS,FS }));
	m_pullAnimation = new Animation(CreateAnimation(*m_sprite, *TextureManager::GetTexture("Player_Spritesheet_noArm.png"), 1, 0.f, { FS,FS * 4,FS,FS }));
	m_attackSideAnim = new Animation(CreateAnimation(*m_sprite, *TextureManager::GetTexture("Player_sideAttack.png"), 10, m_stats->m_attackSideSpeed, { 0,0,350,FS }));
	m_attackSideAnim->loop = false;
	m_attackUpAnim = new Animation(CreateAnimation(*m_sprite, *TextureManager::GetTexture("Player_upAttack.png"), 6, m_stats->m_attackSideSpeed, { 0,0,200, 400 }));
	m_attackDownAnim = new Animation(CreateAnimation(*m_sprite, *TextureManager::GetTexture("Player_upAttack.png"), 6, m_stats->m_attackSideSpeed, { 0,0,200, 400 }));

	m_sprite->setOrigin({ FS / 2.f, 345.f / 2.f });
	m_sprite->setScale(SPRITE_SCALE);
}

void CharaControl::Update()
{
	const float dt = GameData::DeltaTime * GameData::GameSpeed;
	//m_grassEmitter->setParticlePosition(GetFootPos());

	m_grassSystem->Update(dt);
	m_dirtSystem->Update(dt);

	//if (m_jumpFxSystem->AnimEnded())
	//{
		//m_jumpFxEmitter->Stop();
	//}

	UpdateCollid();
	UpdateCheatsCode();

	m_timerAttackCooldown += dt;
	if (m_timerAttackCooldown >= m_stats->m_attackCooldown)
	{
		m_timerAttackCooldown = m_stats->m_attackCooldown;
	}

	m_timerGrabhookCooldown += dt;
	if (m_timerGrabhookCooldown >= m_stats->m_grabHookStats->cooldown)
	{
		m_timerGrabhookCooldown = m_stats->m_grabHookStats->cooldown;
	}

	m_rayCastShadow->Cast(GetPos(), -_PI / 2.f, 10000.f);

	UpdateInputs();

	switch (m_state)
	{
	case WAIT:

		if (!m_isFeetColide) ChangeState(FALL);
		else if ((m_runRightPressed or m_runLeftPressed) and m_isFeetColide and !m_oppositeDirectionPressed) ChangeState(RUN_SIDE);
		else if (m_jumpPressed and m_isFeetColide) ChangeState(JUMP);
		else if (m_attackUpPressed) ChangeState(ATTACK_UP);
		else if (m_attackSidePressed) ChangeState(ATTACK_SIDE);
		else if (m_grabHookPressed) ChangeState(GRAP_HOOK_SHOT);
		break;

	case RUN_SIDE:

		if (m_runLeftPressed)
		{
			MoveSides(LEFT);
			TurnSides(LEFT);
		}
		else if (m_runRightPressed)
		{
			MoveSides(RIGHT);
			TurnSides(RIGHT);
		}

		if (!m_isFeetColide) ChangeState(FALL);
		else if (m_jumpPressed and m_isFeetColide) ChangeState(JUMP);
		else if (m_oppositeDirectionPressed or (!m_runLeftPressed and !m_runRightPressed)) ChangeState(WAIT);
		else if (m_attackUpPressed) ChangeState(ATTACK_UP);
		else if (m_attackSidePressed) ChangeState(ATTACK_SIDE);
		else if (m_grabHookPressed) ChangeState(GRAP_HOOK_SHOT);

		break;

	case ATTACK_SIDE:

		if (/*(m_runLeftPressed or m_runRightPressed) and*/ m_animator->GetCurrentFrame() > 4)
		{
			if (m_sensor[S_ATTACK] == nullptr)
			{
				m_sensor[S_ATTACK] = new b2::Sensor(*m_hitbox, m_hitboxAttackSideOffset, m_stats->m_attackSize, "Attack", (uintptr_t)this);
			}
			m_hitbox->ApplyLinearImpulse({ m_numDir.x * m_stats->m_attackImpulse, 0.f });
		}

		b2Vec2 vel = m_hitbox->GetBody()->GetLinearVelocity();
		m_hitbox->GetBody()->SetLinearVelocity({ vel.x,vel.y / 2.f });

		if (m_animator->LoopedOnce())
		{
			m_hitbox->GetBody()->SetLinearVelocity({ 0,0 });

			delete m_sensor[S_ATTACK];
			m_sensor[S_ATTACK] = nullptr;

			m_isAttacking = false;

			ChangeState(WAIT);
		}
		break;
	case ATTACK_UP:
		m_hitbox->ApplyLinearImpulse({ 0.f, -m_stats->m_attackImpulse });

		m_upAttackTimer += dt;
		if (m_upAttackTimer >= m_stats->m_attackUpDuration)
		{
			m_upAttackTimer = 0;
			m_hitbox->GetBody()->SetLinearVelocity({ 0,0 });

			delete m_sensor[S_ATTACK];
			m_sensor[S_ATTACK] = nullptr;

			m_isAttacking = false;

			ChangeState(WAIT);
		}
		break;
	case ATTACK_DOWN:
		m_hitbox->ApplyLinearImpulse({ 0.f, m_stats->m_attackImpulse });

		m_downAttackTimer += dt;
		if (m_downAttackTimer >= m_stats->m_attackDownDuration)
		{
			m_downAttackTimer = 0;
			m_hitbox->GetBody()->SetLinearVelocity({ 0,0 });

			delete m_sensor[S_ATTACK];
			m_sensor[S_ATTACK] = nullptr;

			m_isAttacking = false;

			ChangeState(WAIT);
		}
		break;
	case GRAP_HOOK_SHOT:
	{
		m_grabHook->Shoot();
		ChangeState(WAIT);
	}
	break;
	case GRAP_HOOK_PULL:
		if (m_grabHook->GetState() == GrabHook::RETRACT)
		{
			UpdateSlideOnWall(dt);

			//if (m_grapHook->GetGrabHeadPos().x > m_hitbox->GetPosition().x)
			//{
			//	TurnSides(RIGHT);
			//}
			//else
			//{
			//	TurnSides(LEFT);
			//}

			if (m_runLeftPressed)
			{
				TurnSides(LEFT);

				float angle = m_grabHook->GetArm()->GetAngleRad() + _PI / 2.f;
				m_hitbox->GetBody()->ApplyLinearImpulseToCenter({ m_stats->m_walkSpeed * cos(angle), m_stats->m_walkSpeed * sin(angle) }, true);
			}
			else if (m_runRightPressed)
			{
				TurnSides(RIGHT);

				float angle = m_grabHook->GetArm()->GetAngleRad() - _PI / 2.f;
				m_hitbox->GetBody()->ApplyLinearImpulseToCenter({ m_stats->m_walkSpeed * cos(angle), m_stats->m_walkSpeed * sin(angle) }, true);

			}
		}

		if (m_grabHook->GetState() != GrabHook::RETRACT)
		{
			ChangeState(WAIT);
		}

		if (m_attackUpPressed)
		{
			ChangeState(ATTACK_UP);
			if (m_grabHook->GetState() != GrabHook::AIM)
			{
				m_grabHook->SetState(GrabHook::DESTROY);
			}
		}
		else if (m_attackDownPressed)
		{
			ChangeState(ATTACK_DOWN);
			if (m_grabHook->GetState() != GrabHook::AIM)
			{
				m_grabHook->SetState(GrabHook::DESTROY);
			}
		}
		else if (m_attackSidePressed)
		{
			ChangeState(ATTACK_SIDE);
			if (m_grabHook->GetState() != GrabHook::AIM)
			{
				m_grabHook->SetState(GrabHook::DESTROY);
			}
		}

		break;
	case JUMP:

		if (m_runLeftPressed)
		{
			MoveAirSides(LEFT);
			TurnSides(LEFT);
		}
		else if (m_runRightPressed)
		{
			MoveAirSides(RIGHT);
			TurnSides(RIGHT);
		}

		m_jumpTimeToApex += dt;
		if ((m_jumpPressed and m_jumpTimeToApex < m_stats->m_jumpMaxTime) or m_jumpTimeToApex < m_stats->m_jumpMinTime)
		{
			if (m_hitbox->GetVelocity().y > m_stats->m_jumpMaxVelocity)
			{
				float impulse = -m_hitbox->GetBody()->GetMass() * m_stats->m_jumpStrenght * dt;
				m_hitbox->ApplyLinearImpulse({ 0, impulse });
			}
		}

		//Change state
		else ChangeState(FALL);

		if (m_attackUpPressed) ChangeState(ATTACK_UP);
		else if (m_attackDownPressed) ChangeState(ATTACK_DOWN);
		else if (m_attackSidePressed) ChangeState(ATTACK_SIDE);
		else if (m_grabHookPressed) ChangeState(GRAP_HOOK_SHOT);

		break;
	case FALL:
		m_jumpPermission = false;

		if (m_runLeftPressed) { MoveAirSides(LEFT); TurnSides(LEFT); }
		else if (m_runRightPressed) { MoveAirSides(RIGHT); TurnSides(RIGHT); }

		//Change state
		if (m_isFeetColide)
		{
			std::queue<Animation*> animationQueue;
			animationQueue.push(m_fallContactAnimation);
			m_animator->SetAnimationQueue(animationQueue);
			SoundManager::GetInstance()->Play("Lay");
			if (m_runLeftPressed)
			{
				ChangeState(RUN_SIDE);
			}
			else if (m_runRightPressed)
			{
				ChangeState(RUN_SIDE);
			}
			else
			{
				ChangeState(WAIT);
			}
		}
		else if (m_hitbox->GetBody()->GetLinearVelocity().y < 0)
		{
			std::queue<Animation*> animationQueue;
			animationQueue.push(m_fallAnimation);
			m_animator->SetAnimationQueue(animationQueue);
		}

		if (m_attackUpPressed) ChangeState(ATTACK_UP);
		else if (m_attackDownPressed) ChangeState(ATTACK_DOWN);
		else if (m_attackSidePressed) ChangeState(ATTACK_SIDE);
		else if (m_grabHookPressed) ChangeState(GRAP_HOOK_SHOT);

		break;
	case INTERACT:
		break;
	case INVULNERABLE:
		break;
	case HURT:
		break;
	case DEAD:

		break;
	default:
		break;
	}
	if (m_isFeetColide && m_state == RUN_SIDE) 
	{ 
		SoundManager::GetInstance()->Play("RollingPlayer", true);
		SoundManager::GetInstance()->SetVolumeAccordingToValue("RollingPlayer", 5.f);
	}
	else SoundManager::GetInstance()->Stop("RollingPlayer");
	if (m_stats->GetHp() <= 0)
	{
		ChangeState(DEAD);
	}

	if (m_grabHook->GetState() == GrabHook::RETRACT)
	{
		ChangeState(GRAP_HOOK_PULL);
	}

	if (m_isFeetColide and !m_jumpPermission)
	{
		m_jumpTimeToApex = 0.f;
		m_midAirAccel = 0.f;
		m_jumpPermission = true;
	}
	if (m_isHeadColide)
	{
		b2Vec2 vel = m_hitbox->GetBody()->GetLinearVelocity();
		if (vel.y > 0)
		{
			vel.y = 0;
			m_hitbox->GetBody()->SetLinearVelocity(vel);
		}
		if (m_state == JUMP)
		{
			ChangeState(FALL);
		}
	}

	m_grabHook->Update(dt);

	b2::MyContactListener* listener = GameData::World->GetListener();
	b2::CollidData* colData = nullptr;

	colData = listener->GetCollision("Chara-Pick");

	if (m_state != RUN_SIDE)
	{
		m_grassEmitter->Stop();
		m_dirtEmitter->Stop();
	}

}

void CharaControl::ChangeState(State _newState)
{
	std::queue<Animation*> animationQueue;

	if (m_state == ATTACK_DOWN)
	{
		const Vector2f scale = m_sprite->getScale();
		m_sprite->setScale(scale.x, abs(scale.y));
	}

	if (m_state == ATTACK_DOWN or m_state == ATTACK_SIDE or m_state == ATTACK_UP)
	{
		m_timerAttackCooldown = 0.f;
	}

	if (m_state == GRAP_HOOK_PULL)
	{
		m_timerGrabhookCooldown = 0.f;
	}

	switch (_newState)
	{
	case CharaControl::WAIT:
	{
		if (m_state == RUN_SIDE)
			animationQueue.push(m_endMoveAnimation);
		animationQueue.push(m_idleAnimation);
	}
	break;
	case CharaControl::RUN_SIDE:
	{
		animationQueue.push(m_beginMoveAnimation);
		animationQueue.push(m_moveAnimation);
		if (GameData::World->GetListener()->GetCollision("CharaFeet-Platform")->isContactAtLeastOne)
		{
			m_grassEmitter->Start();
			m_dirtEmitter->Start();
		}

		break;
	}
	case CharaControl::ATTACK_SIDE:
		if (m_timerAttackCooldown >= m_stats->m_attackCooldown)
		{
			animationQueue.push(m_attackSideAnim);
			SoundManager::GetInstance()->Play("DrillAttack");


			m_isAttacking = true;
			m_hitbox->GetBody()->SetLinearVelocity({ 0,0 });
		}
		else
		{
			_newState = m_state;
		}
		break;
	case ATTACK_UP:
		if (m_timerAttackCooldown >= m_stats->m_attackCooldown)
		{

			animationQueue.push(m_attackUpAnim);
			SoundManager::GetInstance()->Play("DrillAttack");

			m_sensor[S_ATTACK] = new b2::Sensor(*m_hitbox, m_hitboxAttackUpOffset, m_stats->m_attackSize, "Attack", (uintptr_t)this);
			m_isAttacking = true;
			m_hitbox->GetBody()->SetLinearVelocity({ 0,0 });
		}
		else
		{
			_newState = m_state;
		}
		break;

	case ATTACK_DOWN:
	{
		if (m_timerAttackCooldown >= m_stats->m_attackCooldown)
		{
			const Vector2f scale = m_sprite->getScale();
			m_sprite->setScale(scale.x, -abs(scale.y));

			animationQueue.push(m_attackDownAnim);
			SoundManager::GetInstance()->Play("DrillAttack");

			m_sensor[S_ATTACK] = new b2::Sensor(*m_hitbox, m_hitboxAttackDownOffset, m_stats->m_attackSize, "Attack", (uintptr_t)this);
			m_isAttacking = true;
			m_hitbox->GetBody()->SetLinearVelocity({ 0,0 });
		}
		else
		{
			_newState = m_state;
		}
		break;
	}
	case CharaControl::GRAP_HOOK_SHOT:
		if (m_timerGrabhookCooldown >= m_stats->m_grabHookStats->cooldown)
		{

		}
		else
		{
			_newState = m_state;
		}
		break;
	case CharaControl::GRAP_HOOK_PULL:
		animationQueue.push(m_pullAnimation);
		break;
	case CharaControl::JUMP:
	{
		animationQueue.push(m_jumpAnimation);
		SoundManager::GetInstance()->Play("Jump");

		sf::Sprite* sprite = new sf::Sprite();
		sprite->setOrigin(100, 50);
		sprite->setTextureRect({ 0, 0, 200, 100 });
		sf::Texture* texture = TextureManager::GetTexture("JumpFX.png");
		Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 4, 15.f, { 0,0,200,100 }));
		animation->loop = false;
		ParticleManager::AddParticle(new StandardParticle(0.f, 0.f, 1.f, GetFootPos(), animation));
		break;
	}
	case CharaControl::FALL:

		break;
	case CharaControl::INTERACT:
		break;
	case CharaControl::INVULNERABLE:
		break;
	case CharaControl::HURT:
		break;
	case CharaControl::DEAD:
	{
		sf::Texture texture = *TextureManager::GetTexture("Player_Spritesheet_Mort.png");
		sf::Sprite* sprite = GetSprite();
		Animation* animation = new Animation(CreateAnimation(*sprite, texture, 12, 9.f, { 0,0,250,250 }));
		std::queue<Animation*> queue;
		queue.push(animation);
		GetAnimator()->SetAnimationQueue(queue);

		LoadingState::StartLoading<GameOverState>();
		GameOverState::PushState(new FadeOutState(new LoadingState()));
		break;
	}
	default:
		break;
	}

	m_animator->SetAnimationQueue(animationQueue);
	m_state = _newState;
}

void CharaControl::UpdateState()
{
	if (true)
	{

	}
}

void CharaControl::UpdateInputs()
{
	InputManager& input = *InputManager::GetInstance();

	if (m_canControl)
	{
		if (m_canMove)
		{
			m_oppositeDirectionPressed = input.IsLongPress("MoveLeft") and input.IsLongPress("MoveRight");
			m_runLeftPressed = input.IsLongPress("MoveLeft");
			m_runRightPressed = input.IsLongPress("MoveRight");
		}
		if (m_canJump)
		{
			m_jumpPressed = input.IsLongPress("Jump");
		}
		if (m_canAttackUp)
		{
			m_attackUpPressed = input.IsSinglePress("Attack") and input.IsLongPress("ShowUp");
		}
		if (m_canAttackDown)
		{
			m_attackDownPressed = input.IsSinglePress("Attack") and input.IsLongPress("ShowDown");
		}
		if (m_canAttackSide)
		{
			m_attackSidePressed = input.IsSinglePress("Attack");
		}
		if (m_canShootGrapple)
		{
			m_grabHookPressed = input.IsSinglePress("GrabHook");
		}
		if (m_canInteract)
		{
			m_interactPressed = input.IsSinglePress("Interact");
		}
	}
}

void CharaControl::UpdateAnimation()
{

}

void CharaControl::Display(RenderWindow& _window)
{
	float length = m_rayCastShadow->GetDynamicLength();
	if (length > 500.f)
	{
		length = 500.f;
	}
	float alpha = RuleOfThree(length, 0.f, 500.f, 200.f, 0.f);
	float scaleX = RuleOfThree(length, 0.f, 500.f, 0.3f, 1.f);

	sf::Color color;
	color.a = (sf::Uint8)(alpha);
	m_rayCastShadow->SetHiterColor(color);

	m_rayCastShadow->SetHiterScale(scaleX, 1.f);
	m_rayCastShadow->Display(_window);

	m_grabHook->Display(_window);
	m_animator->Display(_window);

	if (m_isInvincible)
	{
		m_invincibleTimer += GameData::DeltaTime;
		int timer = (int)(m_invincibleTimer * BLINK_SPEED);
		m_sprite->setColor(timer % 2 ? BLINK_COLOR : sf::Color::White);

		if (m_invincibleTimer > INVULNERABILITY_TIME)
		{
			m_invincibleTimer = 0;
			m_isInvincible = false;
			m_sprite->setColor(sf::Color::White);
		}
	}

	m_grassSystem->Display(_window);
	m_dirtSystem->Display(_window);
}

void CharaControl::TurnSides(Direction _dir)
{
	if (_dir == LEFT and m_direction.x != LEFT)
	{
		const Vector2f scale = m_sprite->getScale();
		m_sprite->setScale(-abs(scale.x), scale.y);
		m_direction.x = LEFT;
		m_numDir.x = -1;
		m_midAirAccel = 0.f;
		m_hitboxAttackSideOffset.x = -abs(m_hitboxAttackSideOffset.x);
		if (m_state == RUN_SIDE)
		{
			m_grassSystem->ClearAffectors();
			m_grassSystem->ApplyForceOnParticles(200.f, 0.f);
			m_grassSystem->FadeParticles(0.f, 0.15f);

			m_grassSystem->FadeParticles(0.f, 0.15f);
		}
	}

	else if (_dir == RIGHT and m_direction.x != RIGHT)
	{
		const Vector2f scale = m_sprite->getScale();
		m_sprite->setScale(abs(scale.x), scale.y);
		m_direction.x = RIGHT;
		m_numDir.x = 1;
		m_midAirAccel = 0.f;
		m_hitboxAttackSideOffset.x = abs(m_hitboxAttackSideOffset.x);
		if (m_state == RUN_SIDE)
		{
			m_grassSystem->ClearAffectors();
			m_grassSystem->ApplyForceOnParticles(-200.f, 0.f);
			m_grassSystem->FadeParticles(0.f, 0.15f);

			m_dirtSystem->FadeParticles(0.f, 0.15f);
		}
	}
}

void CharaControl::MoveSides(Direction _dir)
{
	b2Vec2 velocity = m_hitbox->GetBody()->GetLinearVelocity();
	float desiredVel = 0;

	switch (_dir)
	{
	case LEFT:  desiredVel = velocity.x - m_stats->m_maxWalkSpeed; break;
	case NO_DIR:  desiredVel = 0; break;
	case RIGHT: desiredVel = velocity.x + m_stats->m_maxWalkSpeed; break;
	}

	if (desiredVel > m_stats->m_maxWalkSpeed)
	{
		desiredVel = m_stats->m_maxWalkSpeed;
	}
	else if (desiredVel < -m_stats->m_maxWalkSpeed)
	{
		desiredVel = -m_stats->m_maxWalkSpeed;
	}

	float velChange = desiredVel - velocity.x;
	if (std::abs(velChange) >= m_stats->m_walkSpeed)
	{
		velChange = velChange > 0 ? m_stats->m_walkSpeed : -m_stats->m_walkSpeed;
	}

	float impulse = m_hitbox->GetBody()->GetMass() * velChange;
	m_hitbox->ApplyLinearImpulse({ impulse, 0 });

}

void CharaControl::MoveAirSides(Direction _dir)
{
	b2Vec2 velocity = m_hitbox->GetBody()->GetLinearVelocity();
	float desiredVel = 0;

	/*if (m_midAirAccel < m_stats->m_midAirMaxSpeed)
	{
		m_midAirAccel += GameData::DeltaTime * m_stats->m_midAirSpeed;
	}*/

	switch (_dir)
	{
	case LEFT:  desiredVel = velocity.x - m_stats->m_midAirSpeed; break;
	case NO_DIR:  desiredVel = 0; break;
	case RIGHT: desiredVel = velocity.x + m_stats->m_midAirSpeed; break;
	}

	if (desiredVel > m_stats->m_midAirMaxSpeed)
	{
		desiredVel = m_stats->m_midAirMaxSpeed;
	}
	else if (desiredVel < -m_stats->m_midAirMaxSpeed)
	{
		desiredVel = -m_stats->m_midAirMaxSpeed;
	}

	float velChange = desiredVel - velocity.x;

	if (std::abs(velChange) >= m_stats->m_midAirSpeed)
	{
		velChange = velChange > 0 ? m_stats->m_midAirSpeed : -m_stats->m_midAirSpeed;
	}

	float impulse = m_hitbox->GetBody()->GetMass() * velChange;
	m_hitbox->ApplyLinearImpulse({ impulse, 0 });
}

void CharaControl::SetKnockBack(float _angle, float _force)
{
	sf::Vector2f impulse = sf::Vector2f(cos(_angle), sin(_angle)) * _force;
	m_hitbox->ApplyLinearImpulse(impulse);
}


void CharaControl::Attack()
{
	//if (m_attackPressed)
	//{

	//}
}

void CharaControl::Jump()
{

}

void CharaControl::UpdateSlideOnWall(float _dt)
{
	const sf::Vector2f target = m_grabHook->GetTargetList()->back()->worldPos;
	sf::Vector2f impulse;
	//b2::CollidData* colData;

	if (m_isSideColide)
	{
		//colData = GameData::World->GetListener()->GetCollision("CharaLeft-Platform");
		if (GetPos().y < target.y)
		{
			impulse.y = 5000 * _dt;
		}
		else
		{
			impulse.y = -5000 * _dt;
		}
	}

	if (m_isHeadColide)
	{
		if (GetPos().x < target.x)
		{
			impulse.x = 500 * _dt;
		}
		else
		{
			impulse.x = -500 * _dt;
		}
	}

	m_hitbox->ApplyLinearImpulse(impulse);
}


sf::Vector2f CharaControl::GetPos(sf::Vector2f _offset) const
{
	return m_hitbox->GetPosition() + _offset;
}

b2Vec2 CharaControl::GetPosB2() const
{
	return m_hitbox->GetBody()->GetPosition();
}

sf::Vector2f CharaControl::GetFootPos() const
{

	sf::Vector2f pos = m_hitbox->GetPosition();
	pos.y += m_hitbox->GetSize().y / 2.f;
	if (m_sprite->getScale().y < 0.f)
	{
		pos.y -= FS / 2.f;
	}
	return pos;
}

bool CharaControl::GetAttackIsFullCooldown()
{
	return m_timerAttackCooldown >= m_stats->m_attackCooldown;
}

bool CharaControl::GetGrabhookIsFullCooldown()
{
	return m_timerGrabhookCooldown >= m_stats->m_grabHookStats->cooldown;
}

void CharaControl::SetPos(sf::Vector2f _pos)
{
	m_hitbox->SetPosition(_pos);
}

void CharaControl::SetLinearVelocity(sf::Vector2f _vel)
{
	m_hitbox->GetBody()->SetLinearVelocity({ _vel.x, _vel.y });
}

void CharaControl::SetCanControl(bool _canControl)
{
	m_canControl = _canControl;

	if (!_canControl)
	{
		m_oppositeDirectionPressed = false;
		m_runLeftPressed = false;
		m_runRightPressed = false;
		m_attackSidePressed = false;
		m_grabHookPressed = false;
		m_jumpPressed = false;
		m_interactPressed = false;
	}
}

void CharaControl::DestroyGrabhook()
{
	if (m_grabHook->GetState() != GrabHook::AIM)
	{
		m_grabHook->SetState(GrabHook::DESTROY);
	}
}

void CharaControl::AddDamage(int _damage)
{
	if (!m_isInvincible)
	{
		m_stats->AddDamage(_damage);
		m_isInvincible = true;
		m_grabHook->SetState(GrabHook::DESTROY);

		CameraManager::PrepareForShake(0.5f, 2.f, 25.f);
		CameraManager::Shake(0.2f);
	}
}

void CharaControl::UpdateCollid()
{
	b2::MyContactListener* listener = GameData::World->GetListener();
	b2::CollidData* colData = nullptr;

	colData = listener->GetCollision("Chara-Pick");
	if (colData->isContactAtLeastOne)
	{
		AddDamage(1);
	}

	colData = listener->GetCollision("Capsule-Chara");
	if (colData->isContactAtLeastOne)
	{
		if (InputManager::GetInstance()->IsSinglePress("Interact"))
		{
			bool isColliding = WorldManager->isObjectColliding<b2::Rectangle>(colData->ptrFirst, true);
			if (isColliding)
			{
				m_stats->AddHealth(5);

				sf::Sprite* sprite = new sf::Sprite();
				sprite->setOrigin(100, 100);
				sprite->setTextureRect({ 0, 0, 200, 200 });
				sf::Texture* texture = TextureManager::GetTexture("ElectrocutFX.png");
				Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 16, 15.f, { 0,0,200,200 }));
				animation->loop = false;

				GetPositionFunc* func = new GetPositionFunc;
				*func = std::bind(&CharaControl::GetPos, this, sf::Vector2f(-30.f, -20.f));
				ParticleManager::AddParticle(new StandardParticle(0.f, 0.f, 1.f, func, animation));
			}
		}
		else
		{
			MineState::NearTeleporter = true;
			MineState::CloseTeleporterPos = World2Screen(colData->fixtureFirst->GetBody()->GetPosition(), { 0,0 });
		}
	}
}

void CharaControl::UpdateCheatsCode()
{
	InputManager& input = *InputManager::GetInstance();

	m_stats->Update();

	if (input.IsSinglePress("GiveLife"))
	{
		m_stats->SetHp(m_stats->GetMaxHp());
	}
}
