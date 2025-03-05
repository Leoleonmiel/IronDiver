#include "ParticleEnvironement.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "common.h"
#include "CharacterController.h"

#define DUST_NB_FRAME 3
#define BUTTERFLY_NB_FRAME 2
#define FIREFLY_NB_FRAME 4

ParticleEnvironment::ParticleEnvironment()
{
	m_systems[DUST] = new ParticleSystem(TextureManager::GetTexture("DustFX.png"));
	m_systems[BUTTERFLY] = new ParticleSystem(TextureManager::GetTexture("ButterflyFX.png"));
	m_systems[FIREFLY] = new ParticleSystem(TextureManager::GetTexture("FireFlyFX.png"));

	for (int i = 0; i < DUST_NB_FRAME; i++)
		m_systems[DUST]->AddTextureRect({ 141 * i,0,141,140 });

	for (int i = 0; i < BUTTERFLY_NB_FRAME; i++)
		m_systems[BUTTERFLY]->AddTextureRect({ 33 * i,0,33,33 });

	for (int i = 0; i < FIREFLY_NB_FRAME; i++)
		m_systems[FIREFLY]->AddTextureRect({ 54 * i,0,54,58 });

	m_systems[BUTTERFLY]->AnimParticles(ParticleSystem::LOOP, 0.2f, BUTTERFLY_NB_FRAME);
	m_systems[FIREFLY]->AnimParticles(ParticleSystem::LOOP, 0.2f, FIREFLY_NB_FRAME);

	m_emitters[DUST] = new ParticleEmitter(1.f, 10.f);
	m_emitters[BUTTERFLY] = new ParticleEmitter(0.5f, 20.f);
	m_emitters[FIREFLY] = new ParticleEmitter(0.3f, 20.f);

	m_emitters[BUTTERFLY]->SetRandomColor(100, 220);

	m_emitters[DUST]->EmitRandomFrames(0, DUST_NB_FRAME - 1);

	m_emitters[DUST]->SetStartVelocity(100.f, 90);
	m_emitters[BUTTERFLY]->SetStartVelocity(50.f, 90);
	m_emitters[FIREFLY]->SetStartVelocity(30.f, 90);

	m_emitters[DUST]->SetDeflect(60.f);
	m_emitters[BUTTERFLY]->SetDeflect(360.f);
	m_emitters[FIREFLY]->SetDeflect(360.f);


	for (int i = 0; i < NB_PARTICLES; i++)
	{
		m_systems[i]->AddEmitter(m_emitters[i]);
		m_systems[i]->FadeParticles(0.1f, 0.1f);
		m_emitters[i]->Start();

		m_systems[i]->ApplyForceOnParticles(0.f, 2.98f);

	}
	//EmitterPosFunc* func = new EmitterPosFunc;
	//*func = std::bind(&CharaControl::GetPos, GameData::CharaCtrl, sf::Vector2f(0.f, 0.f));
	//m_dustEmitter->SetPosition(func);
	//m_butterflyEmitter->SetPosition(func);
	//m_fireflyEmitter->SetPosition(func);
}

ParticleEnvironment::~ParticleEnvironment()
{
	for (int i = 0; i < NB_PARTICLES; i++)
	{
		delete m_systems[i];
		m_systems[i] = nullptr;

		delete m_emitters[i];
		m_emitters[i] = nullptr;
	}
}

void ParticleEnvironment::Update(float _dt)
{
	sf::Vector2f camCenterPos;
	sf::Vector2f camSize;

	for (int i = 0; i < NB_PARTICLES; i++)
	{
		camCenterPos = CameraManager::GetInstance()->GetView()->getCenter();
		camSize = CameraManager::GetInstance()->GetCameraSize() / 2.f;



		m_emitters[i]->SetRandomPosition(camCenterPos - camSize, camCenterPos + camSize);
		m_systems[i]->Update(_dt);
	}
	//m_systems[BUTTERFLY]->ApplyForceOnParticles(rand() % 100, rand() % 100);

}

void ParticleEnvironment::Display(sf::RenderWindow& _window)
{
	for (int i = 0; i < NB_PARTICLES; i++)
	{
		m_systems[i]->Display(_window);
	}
}
