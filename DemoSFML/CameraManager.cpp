#include "CameraManager.h"
#include "CharacterController.h"
#include <iostream>
#include "GrabHook.h"
#include "Box2D_Shape.h"
#include "Common.h"

CameraManager* CameraManager::Instance = nullptr;
CharaControl* CameraManager::Character = nullptr;

//float Distance(sf::Vector2f _a)
//{
//	return sqrt(_a.x * _a.x + _a.y * _a.y);
//}

float GetSpeedFactor(float _dist) 
{
	float distDamp = std::max(1.f, std::abs(_dist) / CAMERA_DISTANCE_DAMPING);
	float damp = CAMERA_DISTANCE_DAMP_MAX + 
		distDamp *
		distDamp;

	return damp;
}

sf::Vector2f UpdateCameraSpeed(sf::Vector2f _speed, const sf::Vector2f& _charaVelocity)
{
	float dt = GameData::DeltaTime;
	sf::Vector2f norm;
	norm.x = _speed.x >= CAMERA_MAX_SPEED ? 1 : (2 - _speed.x / CAMERA_MAX_SPEED);
	norm.y = _speed.y >= CAMERA_MAX_SPEED_Y ? 1 : (2 - _speed.y / CAMERA_MAX_SPEED_Y);

	_speed.x += dt * (CAMERA_ACCELERATION * norm.x + std::abs(_charaVelocity.x) * 0.8f);
	_speed.y += dt * (CAMERA_ACCELERATION_Y * norm.y + std::abs(_charaVelocity.y) * 0.8f);

	_speed.x = std::min(std::max(CAMERA_MAX_SPEED, std::abs(_charaVelocity.x) * 1.5f), _speed.x);
	_speed.y = std::min(std::max(CAMERA_MAX_SPEED_Y, std::abs(_charaVelocity.y) * 1.5f), _speed.y);

	return _speed;
}

bool SimilarSign(int _a, int _b)
{
	return (_a ^ _b) >= 0;
}

sf::Vector2f GetMousePosViewport()
{
	sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(*GameData::Window));
	mousePos.x = (mousePos.x / (SCREEN_SIZE.x / 2.f)) - 1;
	mousePos.y = (mousePos.y / (SCREEN_SIZE.y / 2.f)) - 1;
	return mousePos;
}

void CameraManager::UpdateLookAhead()
{
	b2Vec2 velocity = Character->GetHitbox()->GetBody()->GetLinearVelocity();
	sf::Vector2f path = Character->GetPos() - m_basePos;
	sf::Vector2f viewportCharPos = path;
	sf::Vector2f screenSize = GetCameraSize();
	viewportCharPos.x /= screenSize.x;
	viewportCharPos.y /= screenSize.y;

	float lookAheadChange = GameData::DeltaTime * GameData::GameSpeed * m_lookAheadSpeed;

	if (m_lookingRight && m_lookAhead > -m_lowerThreshold)
	{
		m_lookAhead -= lookAheadChange;
		if (m_lookAhead < -m_lowerThreshold)
		{
			m_lookAhead = -m_lowerThreshold;
		}
	}
	else if (!m_lookingRight && m_lookAhead != m_lowerThreshold)
	{
		m_lookAhead += lookAheadChange;
		if (m_lookAhead > m_lowerThreshold)
		{
			m_lookAhead = m_lowerThreshold;
		}
	}

	m_basePos.y = Character->GetPos().y;

	if (m_lookingRight && viewportCharPos.x > m_lookAhead)
	{
		m_basePos = Character->GetPos();
		m_basePos.x -= m_lookAhead * screenSize.x;
	}
	else if (!m_lookingRight && viewportCharPos.x < m_lookAhead)
	{
		m_basePos = Character->GetPos();
		m_basePos.x -= m_lookAhead * screenSize.x;
	}


	if (velocity.x > 0.1f && !m_lookingRight || velocity.x < -0.1f && m_lookingRight)
	{
		//You are going where the camera isn't facing
		if (velocity.x > 0.f)
		{
			if (viewportCharPos.x > m_upperThreshold)
			{
				m_lookAhead = viewportCharPos.x;
				m_lookingRight = true;
			}
		}
		else
		{
			if (viewportCharPos.x < -m_upperThreshold)
			{
				m_lookAhead = viewportCharPos.x;
				m_lookingRight = false;
			}
		}
	}
	else
	{
		//You are not moving
	}

	m_debugText->setString("Character velocity: " + std::to_string(viewportCharPos.x) + ", " + std::to_string(viewportCharPos.y) + "\nLookAhead " + std::to_string(m_lookAhead) + "\nLookAhead Change: " + std::to_string(lookAheadChange));
}

sf::Vector2f CameraManager::GetShakeOffset()
{
	float angle;
	float amplitude;
	float distance;
	float timeScale = GameData::DeltaTime * GameData::GameSpeed;
	float movement = m_shakeSpeed * timeScale;
	sf::Vector2f path;

	if (m_shakeOffset == m_shakeOffsetTarget)
	{
		m_reverseShake = !m_reverseShake;

		angle = m_shakeAngle + m_shakeAngleAmplitude * (((float)rand() / RAND_MAX) - 0.5f);
		/*angle *= 3.14f;*/
		amplitude = m_shakeAmplitude * (((rand() / RAND_MAX) * 0.2f) + 0.8f);
		m_shakeOffsetTarget.x = amplitude * cos(angle);
		m_shakeOffsetTarget.y = amplitude * sin(angle);

		if (m_reverseShake)
		{
			m_shakeOffsetTarget.x *= -1;
			m_shakeOffsetTarget.y *= -1;
		}

		m_shakeOffset = m_shakeOffsetTarget;
	}
	/*else
	{
		path = m_shakeOffsetTarget - m_shakeOffset;
		distance = std::sqrt(DistanceSquared(path, { 0,0 }));

		if (distance * timeScale <= movement)
		{
			m_shakeOffset = m_shakeOffsetTarget;
		}
		else
		{
			path /= distance;
			m_shakeOffset += path * movement;
		}
	}*/

	return m_shakeOffset;
}

void CameraManager::FollowUpdateTarget()
{
	sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(*GameData::Window));
	sf::Vector2f charPos = Character->GetPos();
	float lookAheadPos;
	float screenSpaceLookAhead;
	float distance;

	mousePos -= SCREEN_SIZE / 2.f;

	if (DistanceSquared(mousePos, { 0,0 }) > 150.f * 150.f)
	{
		distance = std::sqrt(DistanceSquared(mousePos, { 0,0 }));
		mousePos /= distance;
		mousePos *= m_aimZoneRadius;
	}

	mousePos *= CAMERA_AIM_FACTOR * m_aimFactor;
	m_aimTarget = mousePos;

	screenSpaceLookAhead = GetCameraSize().x;
	lookAheadPos = screenSpaceLookAhead * -m_lookAhead;

	m_target = charPos;
	m_target.x += lookAheadPos;

	m_staticPosition = m_target;

	m_target += mousePos;
}

void CameraManager::FollowUpdateSpeed()
{
	m_aimCurrentDelay += GameData::GameSpeed * GameData::DeltaTime;

	if (m_aimPosition == m_aimTarget || m_aimCurrentDelay < m_aimDelay)
	{
		if (m_currentSpeed > 0.f)
		{
			m_currentSpeed -= GameData::DeltaTime * GameData::GameSpeed * m_deceleration;
			if (m_currentSpeed < 0.f)
			{
				m_currentSpeed = 0.f;
				m_aimCurrentDelay = 0.f;
			}
		}

		m_cameraOnTarget = true;

		return;
	}

	m_cameraOnTarget = false;

	if (m_currentSpeed < m_maxSpeed)
	{
		m_currentSpeed += GameData::DeltaTime * GameData::GameSpeed * m_acceleration;
		if (m_currentSpeed > m_maxSpeed)
		{
			m_currentSpeed = m_maxSpeed;
		}
	}
}

void CameraManager::FollowUpdatePosition()
{
	sf::Vector2f path = m_aimTarget - m_aimPosition;
	float distance = std::sqrt(DistanceSquared(path, { 0,0 }));
	float movementRange = m_currentSpeed * GameData::DeltaTime * GameData::GameSpeed;
	sf::Vector2f screenSize = GetCameraSize();
	screenSize /= 2.f;

	if (distance <= movementRange)
	{
		m_aimPosition = m_aimTarget;
	}
	else
	{
		path /= distance;
		path *= movementRange;
		m_aimPosition += path;
	}

	m_position = m_basePos + m_aimPosition;

	/*if (m_position.x - screenSize.x < m_topLeft.x)
	{
		m_position.x = m_topLeft.x + screenSize.x;
	}
	else if (m_position.x + screenSize.x > m_bottomRight.x)
	{
		m_position.x = m_bottomRight.x - screenSize.x;
	}

	if (m_position.y - screenSize.y < m_topLeft.y)
	{
		m_position.y = m_topLeft.y + screenSize.y;
	}
	else if (m_position.y + screenSize.y > m_bottomRight.y)
	{
		m_position.y = m_bottomRight.y - screenSize.y;
	}*/
}

void CameraManager::UpdateGrabBias()
{
	if (m_grabBias == 0.f && m_state != GRAPPLING)
	{
		return;
	}

	if (m_grabBias == 1.f && m_state == GRAPPLING)
	{
		return;
	}

	if (m_state == GRAPPLING)
	{
		m_grabBias += GameData::DeltaTime * GameData::GameSpeed;
		if (m_grabBias > 1.f)
		{
			m_grabBias = 1.f;
		}
		return;
	}
	else
	{
		m_grabBias -= GameData::DeltaTime * GameData::GameSpeed;
		if (m_grabBias < 0.f)
		{
			m_grabBias = 0.f;
		}
		return;
	}
}

void CameraManager::UpdateFollow()
{
	if (Character == nullptr)
	{
		return;
	}

	UpdateLookAhead();
	FollowUpdateTarget();
	FollowUpdateSpeed();
	FollowUpdatePosition();

	if (m_transitioning)
	{
		m_transitionTimer += GameData::DeltaTime;
		float percent = m_transitionTimer / m_transitionDuration;
		sf::Vector2f followGoal = GetFollowTarget();
		sf::Vector2f path = followGoal - m_transitionOrigin;

		if (percent >= 1.f)
		{
			percent = 1.f;
			m_transitioning = false;
		}

		m_position = m_transitionOrigin + (path * percent);
	}

	if (m_shakeTimer > 0)
	{
		m_shakeTimer -= GameData::DeltaTime;
		m_position += GetShakeOffset();
	}

	ClampPosition();

	m_view->setCenter(m_position);
	m_window->setView(*m_view);
}

void CameraManager::UpdateGrappling()
{
	//sf::Vector2f path;
	//GrabHook& grabHook = *Character->GetGrapHook();
	//std::deque<b2::Shape*>* hookList = grabHook.GetHookList();
	//sf::Vector2f hookPos = (*hookList)[0]->GetPosition();
	//sf::Vector2f middle;

	//path = hookPos - Character->GetPos();

	//float dx = std::abs(path.x) / SCREEN_SIZE.x;
	//float dy = std::abs(path.y) / SCREEN_SIZE.y;
	//float max = dx > dy ? dx : dy;

	//m_grabZoom = max + 0.2f;

	//float lookAheadSpace = GetCameraSize().x * m_lookAheadAmplitude * m_lookAhead;

	//m_position = Character->GetPos();
	//m_position.x += lookAheadSpace;

	//m_aimTarget = { 0,0 };
	//
	//path = m_aimTarget - m_aimPosition;
	//float distance = std::sqrt(DistanceSquared(path, { 0,0 }));
	//float movementRange = m_currentSpeed * GameData::DeltaTime * GameData::GameSpeed;

	//if (distance <= movementRange)
	//{
	//	m_aimPosition = m_aimTarget;
	//}
	//else
	//{
	//	path /= distance;
	//	path *= movementRange;
	//	m_aimPosition += path;
	//}

	//m_position += m_aimPosition;

	//if (m_position.x < m_topLeft.x)
	//{
	//	m_position.x = m_topLeft.x;
	//}
	//else if (m_position.x > m_bottomRight.x)
	//{
	//	m_position.x = m_bottomRight.x;
	//}

	//if (m_position.y < m_topLeft.y)
	//{
	//	m_position.y = m_topLeft.y;
	//}
	//else if (m_position.y > m_bottomRight.y)
	//{
	//	m_position.y = m_bottomRight.y;
	//}

	//m_view->setCenter(m_position);
	//m_window->setView(*m_view);
}

void CameraManager::UpdateFixed()
{
	float deltaTime = GameData::DeltaTime;
	sf::Vector2f path;
	sf::Vector2f target;
	if (m_state == FIXED)
	{
		target = m_fixedTarget;
	}
	else
	{
		target = m_cameraZoneFixedTarget;
	}

	path = target - m_position;
	m_speed = UpdateCameraSpeed(m_speed, {0,0});
	sf::Vector2f trueSpeed;
	trueSpeed.x = m_speed.x * GetSpeedFactor(path.x);
	trueSpeed.y = m_speed.y * GetSpeedFactor(path.y);
	sf::Vector2f direction = (path / Distance(path));

	if (std::abs(path.x) < trueSpeed.x * deltaTime)
	{
		m_position.x = target.x;
		m_speed.x = 0;
	}
	else
	{
		direction.x *= trueSpeed.x * deltaTime;
		m_position.x += direction.x;
		m_position.x = std::round(m_position.x);
		m_followActive = true;
	}

	if (std::abs(path.y) < trueSpeed.y * deltaTime)
	{
		m_position.y = target.y;
		m_speed.y = 0;
	}
	else
	{
		direction.y *= trueSpeed.y * deltaTime;
		m_position.y += direction.y;
		m_position.y = std::round(m_position.y);
		m_followActive = true;
	}

	if (m_shakeTimer > 0)
	{
		m_shakeTimer -= GameData::DeltaTime;
		m_position += GetShakeOffset();
	}

	ClampPosition();

	m_view->setCenter(m_position);
	m_window->setView(*m_view);
}

void CameraManager::UpdateZoom()
{
	float zoomGoal = m_zoneZoom ?  m_zoneZoomGoal : m_zoomGoal;
	float deltaZoom;
	char sign;
	float zoomChange;

	sf::Vector2f screenSize = SCREEN_SIZE;
	sf::Vector2f screenLockSize{ m_bottomRight.x - m_topLeft.x, m_bottomRight.y - m_topLeft.y };
	sf::Vector2f zoomLevel = { screenLockSize.x / SCREEN_SIZE.x, screenLockSize.y / SCREEN_SIZE.y};
	float zoomLock = std::min(zoomLevel.x, zoomLevel.y);
	if (zoomGoal > zoomLock)
	{
		zoomGoal = zoomLock;
		m_currentZoom = zoomGoal;
		m_view->setSize(SCREEN_SIZE * m_currentZoom);
	}

	if (m_currentZoom == zoomGoal)
	{
		return;
	}

	deltaZoom = zoomGoal - m_currentZoom;
	sign = deltaZoom > 0 ? 1 : -1;
	zoomChange = m_zoomSpeed * GameData::DeltaTime;

	if (std::abs(deltaZoom) <= zoomChange)
	{
		m_currentZoom = zoomGoal;
	}
	else
	{
		m_currentZoom += sign * zoomChange;
	}

	m_view->setSize(SCREEN_SIZE * m_currentZoom);
}

void CameraManager::ClampPosition()
{
	sf::Vector2f screenSize = GetCameraSize();
	screenSize /= 2.f;

	if (m_position.x - screenSize.x < m_topLeft.x)
	{
		m_position.x = m_topLeft.x + screenSize.x;
	}
	else if (m_position.x + screenSize.x > m_bottomRight.x)
	{
		m_position.x = m_bottomRight.x - screenSize.x;
	}

	if (m_position.y - screenSize.y < m_topLeft.y)
	{
		m_position.y = m_topLeft.y + screenSize.y;
	}
	else if (m_position.y + screenSize.y > m_bottomRight.y)
	{
		m_position.y = m_bottomRight.y - screenSize.y;
	}

}

CameraManager* CameraManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new CameraManager();
	}

	return Instance;
}

void CameraManager::SetCameraFixed(bool _fixed)
{
	if (_fixed && Instance->m_state != CAMERASTATE::FIXED)
	{
		Instance->m_state = CAMERASTATE::FIXED;
		Instance->m_speed = { 0,0 };
	}
	else if (!_fixed && Instance->m_state == CAMERASTATE::FIXED)
	{
		Instance->m_state = FOLLOW;
		Instance->ResetLookAhead();
		Instance->m_speed = { 0,0 };
		Instance->m_transitioning = true;
		Instance->m_transitionOrigin = Instance->m_fixedTarget;
		Instance->m_transitionTimer = 0.f;
	}
}

void CameraManager::SetFixedCameraTarget(sf::Vector2f _pos)
{
	Instance->m_fixedTarget = _pos;
}

void CameraManager::SetCameraZoneFixedView(bool _fixed)
{
	if (!_fixed && Instance->m_cameraZoneFixedView)
	{
		Instance->m_transitioning = true;
		Instance->m_transitionOrigin = Instance->m_cameraZoneFixedTarget;
		Instance->m_transitionTimer = 0.f;
	}

	Instance->m_cameraZoneFixedView = _fixed;
}

void CameraManager::PrepareForShake(float _angle, float _angleAmplitude, float _amplitude)
{
	Instance->m_shakeAngle = _angle;
	Instance->m_shakeAngleAmplitude = _angleAmplitude;
	Instance->m_shakeAmplitude = _amplitude;
}

void CameraManager::Shake(float _time)
{
	Instance->m_shakeTimer = _time;
}

CameraManager::CameraManager()
{
	if (Character != nullptr)
	{
		m_position = Character->GetPos();
	}
	else
	{
		m_position = sf::Vector2f(0, 0);
	}

	m_topLeft = { -20000,-20000 };
	m_bottomRight = { 2000000,200000 };
	
	m_target = m_position;
	m_currentZoom = 1.f;

	m_window = GameData::Window;
	m_view = new sf::View(m_target, SCREEN_SIZE * m_currentZoom);

	m_state = FOLLOW;
	m_prevState = FOLLOW;

	m_debugFont = new sf::Font();
	m_debugText = new sf::Text();

	m_debugFont->loadFromFile("Assets/Fonts/ARIAL.TTF"); 
	m_debugText->setFont(*m_debugFont);
}

CameraManager::~CameraManager()
{
	delete m_view;
}

bool CameraManager::ReachedGoal()
{
	sf::Vector2f goal = (m_state == FIXED ? GetFixedTarget() : GetFollowTarget());

	//std::cout << "Goal is " << goal.x << " " << goal.y << "\n\tCurrently at " << m_position.x << " " << m_position.y << std::endl;

	return m_position == goal;
}

void CameraManager::Update(void)
{
	m_posDelta = Character->GetPos() - m_oldCharPos;
	
	switch (m_state)
	{
	case FOLLOW:
		if (m_cameraZoneFixedView)
		{
			UpdateFixed();
		}
		else
		{
			UpdateFollow();
		}
		break;
	case CAMERASTATE::FIXED:
		UpdateFixed();
		break;
	default:
		break;
	}
	UpdateZoom();

	m_oldCharPos = Character->GetPos();
}

void CameraManager::DisplayDebugInformation()
{


	sf::CircleShape circleShape;
	sf::RectangleShape rectangleShape;
	rectangleShape.setSize({ 25.f, SCREEN_SIZE.y });
	rectangleShape.setPosition({ SCREEN_SIZE.x * 0.6f, 0.f });
	rectangleShape.setOrigin(12.5f, 0.f);

	sf::Vector2f pos = Character->GetPos() - m_position;
	circleShape.setPosition(SCREEN_SIZE / 2.f + pos);
	circleShape.setRadius(m_aimZoneRadius);
	circleShape.setFillColor({ 200,0,0,100 });
	circleShape.setOrigin({ m_aimZoneRadius, m_aimZoneRadius});
	m_debugText->setPosition(0, 0);
	m_debugText->setCharacterSize(60);
	sf::View view = GameData::Window->getView();
	GameData::Window->setView(GameData::Window->getDefaultView());
	GameData::Window->draw(*m_debugText);
	GameData::Window->draw(circleShape);
	GameData::Window->draw(rectangleShape);
	rectangleShape.setPosition({ SCREEN_SIZE.x * 0.4f, 0.f });
	GameData::Window->draw(rectangleShape);
	GameData::Window->setView(view);
}

void CameraManager::SetCharacter(CharaControl* _character)
{
	Character = _character;
	m_position = Character->GetPos();
	m_basePos = Character->GetPos();

	ClampPosition();
	m_lookAhead = 0.5f;
	m_aimPosition = { 0,0 };

	m_view->setCenter(m_position);
	m_window->setView(*m_view);
}

void CameraManager::ResetLookAhead()
{
	sf::Vector2f path = Character->GetPos() - m_basePos;
	sf::Vector2f viewportCharPos = path;
	viewportCharPos.x /= GetCameraSize().x;
	viewportCharPos.y /= GetCameraSize().y;

	m_lookAhead = viewportCharPos.x;
}

sf::Vector2f CameraManager::GetFollowTarget()
{
	sf::Vector2f target;
	sf::Vector2f screenSize = GetCameraSize();
	screenSize /= 2.f;
	target = m_target;

	if (target.x - screenSize.x < m_topLeft.x)
	{
		target.x = m_topLeft.x + screenSize.x;
	}
	else if (target.x + screenSize.x > m_bottomRight.x)
	{
		target.x = m_bottomRight.x - screenSize.x;
	}

	if (target.y - screenSize.y < m_topLeft.y)
	{
		target.y = m_topLeft.y + screenSize.y;
	}
	else if (target.y + screenSize.y > m_bottomRight.y)
	{
		target.y = m_bottomRight.y - screenSize.y;
	}

	return target;
}

sf::Vector2f CameraManager::GetFixedTarget()
{
	sf::Vector2f target;
	sf::Vector2f screenSize = GetCameraSize();
	screenSize /= 2.f;
	target = m_fixedTarget;

	if (target.x - screenSize.x < m_topLeft.x)
	{
		target.x = m_topLeft.x + screenSize.x;
	}
	else if (target.x + screenSize.x > m_bottomRight.x)
	{
		target.x = m_bottomRight.x - screenSize.x;
	}

	if (target.y - screenSize.y < m_topLeft.y)
	{
		target.y = m_topLeft.y + screenSize.y;
	}
	else if (target.y + screenSize.y > m_bottomRight.y)
	{
		target.y = m_bottomRight.y - screenSize.y;
	}

	return target;
}
