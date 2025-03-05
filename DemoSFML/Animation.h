#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>


struct Animation
{
	sf::Texture* textureList = nullptr;
	sf::Sprite* sprite = nullptr;
	sf::IntRect firstFrame;

	float speed = 0.f;
	unsigned nbFrame = 0;
	int loopCount = -1;

	bool loop = true;
	bool resetTimerBefore = true;
};


#endif

// Creates an animation for a sprite using a single texture (like a sprite-sheet).
Animation CreateAnimation(sf::Sprite& _sprite, sf::Texture _texture, unsigned _nbFrame, float _speed, sf::IntRect _firstFrame);