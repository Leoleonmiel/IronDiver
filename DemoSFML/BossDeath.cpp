#include "BossDeath.h"
#include "common.h"
#include "Entity.h"
#include "LoadingState.h"
#include "FadeoutState.h"
#include "MainMenu.h"
#include "MineState.h"
#include "ParticleSystem.h"
#include "TextureManager.h"
#include "Boss.h"
#include "CameraManager.h"

BossDeath::BossDeath(Entity* _owner, sf::Vector2f _direction) : EntityState(_owner)
{
	float distance = DistanceSquared(_direction, { 0,0 });
	if (distance != 1.f)
	{
		distance = std::sqrt(distance);
		_direction /= distance;
	}

	m_directionNormalised = _direction;
}

void BossDeath::InitState()
{
	m_owner->SetCurrentAnimation("Death");

	MineState::ExplosionSystem = new ParticleSystem(TextureManager::GetTexture("BlowupFX.png"));
	for (int i = 0; i < 14; i++)
	{
		MineState::ExplosionSystem->AddTextureRect(sf::IntRect{ 850 * i,0,850,850 });
	}
	
	ParticleEmitter* emitter = new ParticleEmitter(5.f, 0.75f);
	MineState::ExplosionSystem->AddEmitter(emitter);
	MineState::ExplosionSystem->AnimParticles(ParticleSystem::LIFE_TIME, 6.f, 14);
	MineState::ExplosionEmitter = emitter;
	emitter->Start();

	Boss::BossBeaten = true;
	CameraManager::GetInstance()->SetFixedCameraTarget(m_owner->GetScreenPos());
	CameraManager::GetInstance()->SetCameraFixed(true);
	//CameraManager::GetInstance()->SetCharacter(GameData::CharaCtrl);
}

void BossDeath::Update()
{
	sf::Vector2f pos = m_owner->GetScreenPos();
	float dt = GameData::DeltaTime * (1 + GameData::GameSpeed);
	pos += m_directionNormalised * m_speed * dt;
	m_owner->SetPos(pos);
	m_owner->Move({ 0, 0 });
	m_timer += GameData::DeltaTime;

	if (!m_transition && m_timer > 5.f)
	{
		m_transition = true;
		LoadingState::StartLoading<MainMenu>();
		GameState::PushState(new FadeOutState(new LoadingState(), false));
	}
}

void BossDeath::ExitState()
{

}

EntityState* BossDeath::GetNextState()
{
	return nullptr;
}
