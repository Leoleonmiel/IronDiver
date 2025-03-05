#ifndef SLIDER_H
#define SLIDER_H

#include <SFML\Graphics.hpp>
#include <iostream>

class Slider
{
	sf::Font font;
	sf::Text text;
	int minValue = 0;
	int maxValue = 0;
	int xCord = 0;
	int yCord = 0;
	float maxSize = 0.f;
	float sliderValue = 0.f;
	bool grabbed = false;
	float minSliderPos = 0.f;
	float maxSliderPos = 0.f;
	sf::Sprite* sliderEmpty = nullptr;
	sf::Sprite* sliderFull = nullptr;
	sf::Sprite* sliderHolder = nullptr;
public:
	Slider(int x, int y);
	sf::Text returnText(int x, int y, std::string z, int fontSize);
	void create(int min, int max);
	void Update();
	float getSliderValue();
	void setSliderValue(float newValue);
	void setSliderPercentValue(float newPercentValue);
	void draw(sf::RenderWindow& window);
	void setPosition(int x, int y) { xCord = x; yCord = y; }
	sf::Vector2i getPosition() { return sf::Vector2i(xCord, yCord); }
};

#endif // !SLIDER_H

