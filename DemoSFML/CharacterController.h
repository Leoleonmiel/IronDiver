#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H

#include "SFML/Graphics.hpp"
#include "box2D/box2D.h"

#include <Thor/Particles.hpp>
#include <Thor/Math.hpp>
#include <Thor/Animations.hpp>

namespace b2 { class Rectangle; }
namespace b2 { class Sensor; }
namespace b2 { class DistanceJoint; }
namespace b2 { class RayCastClosest; }
class GrabHook;
class CharacterStats;
class FX;
struct Animation;
class Animator;
class ParticleSystem;
class ParticleEmitter;
class Animator;
enum Direction;

class CharaControl
{
public:

	enum State
	{
		WAIT,
		RUN_SIDE,
		ATTACK_SIDE,
		ATTACK_UP,
		ATTACK_DOWN,
		GRAP_HOOK_SHOT,
		GRAP_HOOK_PULL,
		JUMP,
		FALL,
		INTERACT,
		INVULNERABLE,
		HURT,
		DEAD,
	};

	enum Sensor
	{
		S_HEAD,
		S_FEET,
		S_WALL_L,
		S_WALL_R,
		S_ATTACK,

		NB_SENSOR
	};

	CharaControl(CharacterStats* _stats);
	~CharaControl();

	void LoadAnimations();

	void Update();
	void ChangeState(State _state);
	void UpdateState();
	void UpdateInputs();
	void UpdateAnimation();
	void UpdateCollid();
	void UpdateCheatsCode();

	void Display(sf::RenderWindow& _window);

	void TurnSides(Direction _dir);
	void MoveSides(Direction _dir);
	void MoveAirSides(Direction _dir);
	void SetKnockBack(float _angle, float _force);
	//void Aimed(sf::Vector2f _aimOrigin, float _scope);
	void Attack();
	//void GrapHook();
	void Jump();
	//void Interact();
	void UpdateSlideOnWall(float _dt);

	sf::Vector2i GetDirection() { return m_direction; }
	bool GetIsFeetColide() { return m_isFeetColide; }
	bool GetIsSideColide() { return m_isSideColide; }
	bool GetIsHeadColide() { return m_isHeadColide; }
	bool GetCanHook() { return m_canShootGrapple; }
	bool GetIsInvincible() { return m_isInvincible; }
	State GetState() { return m_state; }
	sf::Vector2f GetPos(sf::Vector2f _offset = {0,0}) const;
	b2Vec2 GetPosB2() const;
	sf::Vector2f GetFootPos() const;
	b2::Rectangle* GetHitbox() { return m_hitbox; }
	CharacterStats* GetStats() { return m_stats; }
	GrabHook* GetGrapHook() { return m_grabHook; }
	sf::Sprite* GetSprite() { return m_sprite; }
	Animator* GetAnimator() { return m_animator; }
	bool GetAttackIsFullCooldown();
	bool GetGrabhookIsFullCooldown();

	void SetIsFeetColide(bool _onGround) { m_isFeetColide = _onGround; }
	void SetIsSideColide(bool _isSideColide) { m_isSideColide = _isSideColide; }
	void SetIsHeadColide(bool _isHeadColide) { m_isHeadColide = _isHeadColide; }
	void SetState(State _state) { m_state = _state; }
	void SetPos(sf::Vector2f _pos);
	void SetLinearVelocity(sf::Vector2f _vel);

	void SetCanJump(bool _canJump) { m_canJump = _canJump; }
	void SetCanMove(bool _canMove) { m_canMove = _canMove; }
	void SetCanAttackSide(bool _canAttackSide) { m_canAttackSide = _canAttackSide; }
	void SetCanAttackUp(bool _canAttackUp) { m_canAttackUp = _canAttackUp; }
	void SetCanAttackDown(bool _canAttackDown) { m_canAttackDown = _canAttackDown; }
	void SetCanShootGrapple(bool _canShootGrapple) { m_canShootGrapple = _canShootGrapple; }
	void SetCanInteract(bool _canInteract) { m_canInteract = _canInteract; }
	void SetCanControl(bool _canControl);
	void DestroyGrabhook();
	void AddDamage(int _damage);

private:

	ParticleSystem* m_grassSystem = nullptr;
	ParticleEmitter* m_grassEmitter = nullptr;

	ParticleSystem* m_dirtSystem = nullptr;
	ParticleEmitter* m_dirtEmitter = nullptr;

	b2::RayCastClosest* m_rayCastShadow = nullptr;

	GrabHook* m_grabHook = nullptr;

	sf::Sprite* m_sprite = nullptr;

	b2::Rectangle* m_hitbox = nullptr;
	b2::Sensor* m_sensor[NB_SENSOR] = { nullptr };
	b2::DistanceJoint* m_sensorJoint[NB_SENSOR] = { nullptr };

	sf::Vector2f m_hitboxAttackSideOffset;
	sf::Vector2f m_hitboxAttackUpOffset;
	sf::Vector2f m_hitboxAttackDownOffset;

	sf::Vector2i m_direction;
	sf::Vector2i m_numDir;

	State m_state = WAIT;

	CharacterStats* m_stats = nullptr;

	Animator* m_animator = nullptr;

	Animation* m_beginMoveAnimation = nullptr;
	Animation* m_endMoveAnimation = nullptr;
	Animation* m_moveAnimation = nullptr;
	Animation* m_idleAnimation = nullptr;
	Animation* m_jumpAnimation = nullptr;
	Animation* m_fallAnimation = nullptr;
	Animation* m_fallContactAnimation = nullptr;
	Animation* m_pullAnimation = nullptr;
	Animation* m_attackSideAnim = nullptr;
	Animation* m_attackUpAnim = nullptr;
	Animation* m_attackDownAnim = nullptr;
	Animation* m_teleportAnimation = nullptr;

	float m_jumpTimeToApex = 0.f;
	float m_midAirAccel = 0.f;
	float m_timerAttackCooldown = 0.f;
	float m_timerGrabhookCooldown = 0.f;
	float m_invincibleTimer = 0.f;
	float m_upAttackTimer = 0.f;
	float m_downAttackTimer = 0.f;

	bool m_isFeetColide = false;
	bool m_isSideColide = false;
	bool m_isHeadColide = false;
	bool m_isAttacking = false;
	bool m_isInvincible = false;

	bool m_oppositeDirectionPressed = false;
	bool m_runLeftPressed = false;
	bool m_runRightPressed = false;
	bool m_attackSidePressed = false;
	bool m_attackUpPressed = false;
	bool m_attackDownPressed = false;
	bool m_grabHookPressed = false;
	bool m_jumpPressed = false;
	bool m_interactPressed = false;

	bool m_jumpPermission = true;

	bool m_canJump = true;
	bool m_canMove = true;
	bool m_canAttackSide = false;
	bool m_canAttackUp = false;
	bool m_canAttackDown = false;
	bool m_canShootGrapple = false;
	bool m_canInteract = true;
	bool m_canControl = true;

	//sf::RectangleShape* m_attackHitbox = nullptr;
};

#endif // !CONTROL_H
