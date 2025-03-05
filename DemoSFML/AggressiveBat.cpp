#include "AggressiveBat.h"
#include "Animator.h"
#include "Animation.h"
#include "TextureManager.h"
#include "EntityIdleFlying.h"

AggressiveBat::AggressiveBat(const EntityDef& _def, sf::Vector2f _pos) : Entity(_def, _pos)
{
	//m_state = new EntityIdleFlying(this);
}

void AggressiveBat::GenerateBaseAnimations()
{
	std::queue<Animation*> animations;
	sf::Sprite* sprite = m_sprite;
	sf::Texture texture;
	Animation* animation;

	GetPositionFunc* func = new GetPositionFunc();
	*func = std::bind(&AggressiveBat::GetScreenPos, this);

	m_animators.push_back(new Animator(func));
	texture = *TextureManager::GetTexture("Fly_Spritesheet.png");
	animation = new Animation(CreateAnimation(*sprite, texture, 2, 24.f, { 0,0,300,300 }));
	animations.push(animation);
	m_animators[1]->SetAnimationQueue(animations);

	animations.pop();

	animation = new Animation(CreateAnimation(*sprite, texture, 4, 4.f, { 0,300,300,300 }));
	sprite->setOrigin(150, 150);
	animations.push(animation);
	m_animators[0]->SetAnimationQueue(animations);
}
