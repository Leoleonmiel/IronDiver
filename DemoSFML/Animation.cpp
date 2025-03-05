#include "Animation.h"
#include <iostream>

Animation CreateAnimation(sf::Sprite& _sprite, sf::Texture _texture, unsigned _nbFrame, float _speed, sf::IntRect _firstFrame)
{
	Animation anim;
	anim.sprite = &_sprite;
	anim.nbFrame = _nbFrame;
	anim.speed = _speed;
	anim.firstFrame = _firstFrame;
	anim.textureList = new sf::Texture[1];
	anim.textureList[0] = _texture;

	return anim;
}
