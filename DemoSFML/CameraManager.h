#pragma once

#include <SFML/Graphics.hpp>

#include "common.h"

#define CAMERA_MAX_SPEED 200.f
#define CAMERA_ACCELERATION 75.f

#define CAMERA_MAX_SPEED_Y 300.0f
#define CAMERA_ACCELERATION_Y 75.0f

#define CAMERA_DISTANCE_DAMPING 50.f
#define CAMERA_DISTANCE_DAMP_MAX 0.4f
#define CAMERA_AIM_FACTOR 0.4f
#define CAMERA_WAKE_UP_DISTANCE 20.f

#define LOOK_AHEAD_X 100
#define LOOK_AHEAD_X_SPEED 100    
#define LOOK_AHEAD_Y 100
#define LOOK_AHEAD_Y_SPEED 100

class CharaControl;


class CameraManager
{
private:
	enum CAMERASTATE
	{
		FIXED,
		FOLLOW,
		GRAPPLING
	};

	sf::Vector2f m_basePos;
	sf::Vector2f m_position;
	sf::Vector2f m_target;
	sf::Vector2f m_fixedTarget;
	sf::Vector2f m_cameraZoneFixedTarget;
	sf::Vector2f m_speed;

	sf::Vector2f m_topLeft;
	sf::Vector2f m_bottomRight;

	sf::Vector2f m_staticPosition;

	float m_zoomSpeed = 1.0f;

	float m_aimFactor = 1.0f;
	float m_aimDelay = 0.3f;
	float m_aimCurrentDelay = 0.f;
	float m_aimZoneRadius = 150.f;

	float m_currentZoom = 1.f;
	float m_zoomGoal = 1.f;
	float m_zoneZoomGoal = 1.f;
	float m_shakeTimer = 0.f;
	float m_shakeAngle = 0.f;
	float m_shakeAngleAmplitude = 0.f;
	float m_shakeAmplitude = 0.f;
	float m_shakeSpeed = 50.f;

	float m_acceleration = 250.0f;
	float m_deceleration = 500.f;
	float m_currentSpeed = 0.f;
	float m_maxSpeed = 250.f;

	float m_grabZoom = 1.0f;
	float m_previousZoom = 1.0f;

	float m_grabBias = 0.f;
	float m_grabCameraSpeed = 500.f;

	float m_lookAhead = 0.f;
	float m_lookAheadSpeed = 0.2f;
	float m_lookAheadAmplitude = 5.f;
	float m_lowerThreshold = 0.1f;
	float m_upperThreshold = 0.2f;

	float m_transitionTimer = 0.f;
	const float m_transitionDuration = 0.5f;

	sf::Vector2f m_transitionOrigin = { 0,0 };
	bool m_transitioning = false;

	/*float m_grabMaxLookAhead = 750.f;
	float m_grabLookAheadSpeed = 1500.f;
	float m_grabCurrentLookAhead = 0.f;*/

	sf::Vector2f m_aimPosition = { 0,0 };
	sf::Vector2f m_aimTarget = { 0,0 };

	bool m_followActive = false;
	bool m_cameraOnTarget = false;
	bool m_lookingRight = true;
	bool m_reverseShake = false;
	bool m_cameraZoneFixedView = false;
	bool m_zoneZoom = false;
	
	sf::Font* m_debugFont = nullptr;
	sf::Text* m_debugText = nullptr;

	//TODO: Replace with Thomas' methods
	sf::Vector2f m_oldCharPos;
	sf::Vector2f m_posDelta;

	sf::Vector2f m_grabTarget;
	sf::Vector2f m_grabCameraSize;

	sf::Vector2f m_shakeOffset;
	sf::Vector2f m_shakeOffsetTarget;

	sf::RenderWindow* m_window;
	sf::View* m_view;

	CAMERASTATE m_state;
	CAMERASTATE m_prevState;

	static CameraManager* Instance;
	static CharaControl* Character;

	void UpdateLookAhead();

	sf::Vector2f GetShakeOffset();

	void FollowUpdateTarget();
	/// <summary>
	/// Must pass after update Target
	/// </summary>
	void FollowUpdateSpeed();
	void FollowUpdatePosition();

	void UpdateGrabBias();

	void UpdateFollow();
	void UpdateGrappling();
	void UpdateFixed();
	void UpdateZoom();

	void ClampPosition();
	
public:
	static CameraManager* GetInstance();
	static void SetCameraFixed(bool);
	static void SetFixedCameraTarget(sf::Vector2f);
	static void SetCameraZoneFixedView(bool _fixed);
	static void SetCameraZoneFixedTarget(sf::Vector2f _target) { Instance->m_cameraZoneFixedTarget = _target; }
	static void SetZoneZoomGoal(float _goal) { Instance->m_zoneZoomGoal = _goal; }
	static void SetZoneZoom(bool _zoom) { Instance->m_zoneZoom = _zoom; }
	static void PrepareForShake(float _angle, float _angleAmplitude, float _amplitude);
	static void Shake(float);
	static void SetZoom(float _zoom) { Instance->m_currentZoom = _zoom; Instance->m_view->setSize(SCREEN_SIZE * Instance->m_currentZoom); }
	static float GetZoom() { return Instance->m_currentZoom; }
	static float GetZoomSpeed() { return Instance->m_zoomSpeed; }

	CameraManager();
	~CameraManager();

	bool ReachedGoal();
	void Update(void);
	void DisplayDebugInformation();

	sf::Vector2f GetCameraGoal() { return m_target; }

	void SetZoomGoal(float _goal) { m_zoomGoal = _goal; }
	void SetZoomSpeed(float _speed) { m_zoomSpeed = _speed; }
	void SetCharacter(CharaControl* _character);
	void SetPosition(sf::Vector2f _pos) { m_target = _pos; m_view->setCenter(_pos); m_window->setView(*m_view); }
	bool ReachedZoomGoal() const { return m_currentZoom == m_zoomGoal; }
	void ResetLookAhead();

	sf::Vector2f GetCameraSize() { return (SCREEN_SIZE * m_currentZoom); }

	sf::Vector2f GetFollowTarget();
	sf::Vector2f GetFixedTarget();
	float GetAimFactor() { return m_aimFactor; }
	sf::View* GetView() { return m_view; }
	void SetAimFactor(float _aimFactor) { m_aimFactor = _aimFactor; }
	void SetBottomLimit(float _y) { m_bottomRight.y = _y; }
	void SetRightLimit(float _x) { m_bottomRight.x = _x; }
	void SetUpLimit(float _y) { m_topLeft.y = _y; }
	void SetLeftLimit(float _x) { m_topLeft.x = _x; }
};

