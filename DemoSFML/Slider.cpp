#include "Slider.h"
#include "common.h"
#include "InputManager.h"
#include "TextureManager.h"

Slider::Slider(int x, int y)
{
	xCord = x;
	yCord = y;

	if (!font.loadFromFile("Assets/Fonts/Orbitron-ExtraBold.ttf")) std::cout << "Error loading font\n";

	text.setFont(font);
	text.setFillColor(sf::Color::White);

	sliderEmpty = TextureManager::CreateSprite("Bare_Audio.png");
	sliderFull = TextureManager::CreateSprite("Volume.png");
	sliderHolder = TextureManager::CreateSprite("Reglage_Volume.png", true);
	sliderEmpty->setPosition((float)x, (float)y);
	sliderFull->setPosition((float)x + 5, (float)y);
	sliderHolder->setPosition((float)x - 5, (float)y);
	sliderEmpty->setOrigin(0.f, (float)sliderEmpty->getGlobalBounds().height / 2.f);
	sliderFull->setOrigin(0.f, (float)sliderFull->getGlobalBounds().height / 2.f);
	maxSize = sliderFull->getGlobalBounds().width;
	minSliderPos = sliderFull->getGlobalBounds().left;
	maxSliderPos = sliderFull->getGlobalBounds().left + sliderFull->getGlobalBounds().width;
}

sf::Text Slider::returnText(int x, int y, std::string z, int fontSize)
{
	text.setCharacterSize(fontSize);
	text.setPosition((float)x, (float)y);
	text.setString(z);
	return text;
}

void Slider::create(int min, int max)
{
	sliderValue = (float)max;
	minValue = min;
	maxValue = max;
}

void Slider::Update()
{
	InputManager* input = InputManager::GetInstance();
	sf::Vector2i topLeft = sf::Vector2i(minSliderPos, (int)(sliderEmpty->getGlobalBounds().top - sliderHolder->getGlobalBounds().height));
	sf::Vector2i bottomRight = sf::Vector2i(maxSliderPos, (int)(sliderEmpty->getGlobalBounds().top + sliderEmpty->getGlobalBounds().height + sliderHolder->getGlobalBounds().height));
	if (input->IsSinglePress("Attack") && GameData::GetMousePos().x >= topLeft.x && GameData::GetMousePos().x <= bottomRight.x && GameData::GetMousePos().y >= topLeft.y && GameData::GetMousePos().y <= bottomRight.y) grabbed = true;
	if (grabbed == true && input->IsLongPress("Attack")) 
	{
		if (GameData::GetMousePos().x >= topLeft.x && GameData::GetMousePos().x <= bottomRight.x && GameData::GetMousePos().y >= topLeft.y && GameData::GetMousePos().y <= bottomRight.y)
		{
			sliderHolder->setPosition(sf::Vector2f(GameData::GetMousePos().x, sliderHolder->getGlobalBounds().top + sliderHolder->getGlobalBounds().height / 2));
			sliderValue = (minValue + ((sliderHolder->getPosition().x - xCord) / (maxSliderPos - minSliderPos) * (maxValue - minValue)));
			sliderValue = (float)static_cast<int>(sliderValue);

			if (sliderValue > maxValue) sliderValue = maxValue;
			if (sliderValue < minValue) sliderValue = minValue;

			sf::IntRect rect = sliderFull->getTextureRect();
			rect.width = maxSize - ((maxValue - sliderValue) / 100 * maxSize);
			sliderFull->setTextureRect(rect);
		}
	}
	if (input->IsLongRelease("Attack")) grabbed = false;
}

float Slider::getSliderValue()
{
	return sliderValue;
}

void Slider::setSliderValue(float newValue)
{
	if (newValue >= minValue && newValue <= maxValue)
	{
		sliderValue = newValue;
		float diff = (float)maxValue - minValue;
		float diff2 = newValue - minValue;
		float zzz = (maxSliderPos - minSliderPos) / diff;
		float posX = zzz * diff2;
		posX += xCord;
		sliderHolder->setPosition(posX, (float)yCord);

		if (sliderValue > maxValue) sliderValue = maxValue;
		if (sliderValue < minValue) sliderValue = minValue;

		sf::IntRect rect = sliderFull->getTextureRect();
		rect.width = maxSize - ((maxValue - sliderValue) / 100 * maxSize);
		sliderFull->setTextureRect(rect);
	}
}

void Slider::setSliderPercentValue(float newPercentValue)
{
	if (newPercentValue >= 0 && newPercentValue <= 100)
	{
		sliderValue = newPercentValue / 100 * maxValue;
		sliderHolder->setPosition(xCord + ((maxSliderPos - minSliderPos) * newPercentValue / 100), (float)yCord);
	}
}

void Slider::draw(sf::RenderWindow& window)
{
	window.draw(returnText(xCord - 10, yCord + 5, "Volume", 20));
	window.draw(*sliderEmpty);
	window.draw(returnText(xCord + sliderEmpty->getGlobalBounds().width - 10, yCord + 5, std::to_string((int)sliderValue), 20));
	window.draw(*sliderFull);
	window.draw(*sliderHolder);
}