#include "Spider.h"
#include "TextureManager.h"
#include "Animation.h"
#include "Animator.h"

Spider::Spider(const EntityDef& _def, sf::Vector2f _pos) : Entity(_def, _pos)
{
	m_state = new SpiderIdle(this);
}

void Spider::HurtboxCollided(b2::UserData* _data)
{
	ChangeState(new SpiderHurt(this));
}

void Spider::GenerateBaseAnimations()
{
	sf::Sprite* sprite = m_sprite;
	sf::Texture* texture = TextureManager::GetTexture("Ray_Spritesheet.png");
	float size = 400.f;
	Animation* animation = new Animation(CreateAnimation(*sprite, *texture, 12, 16.f, { 0,0,(int)size,(int)size }));
	m_animators[0]->AddAnimation("Idle", animation);

	texture = TextureManager::GetTexture("Ray_Death_Spritesheet.png");
	animation = new Animation(CreateAnimation(*sprite, *texture, 8, 16.f, { 0,0,(int)size,(int)size }));
	animation->loop = false;
	m_animators[0]->AddAnimation("Death", animation);

	m_sprite->setOrigin({ size/2.f,size/2.f });
}
