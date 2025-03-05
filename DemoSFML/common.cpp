#include "common.h"
#include "Collectable.h"
#include "Cursor.h"

sf::Clock GameData::Clock;
sf::Time GameData::ClockTime;
sf::RenderWindow* GameData::Window = nullptr;
b2::World* GameData::World = nullptr;
float GameData::DeltaTime = 0;
float GameData::GameTime = 0;
CharaControl* GameData::CharaCtrl = nullptr;
//PlayerInventory* GameData::PlayerInventory = nullptr;
std::list<Collectable*>* GameData::Collectables = nullptr;
float GameData::GameSpeed = 1.0f;
Cursor* GameData::Cursor = nullptr;
CharacterStats* GameData::CharacterStats = nullptr;
bool GameData::fullscreen = false;
//GrabHook* GameData::GrabHook = nullptr;
//SFMLChrono GameData::Chrono;

sf::Sprite LoadSprite(const std::string _path, bool _isCentered)
{
	// Création et chargement de la texture
	sf::Texture* tempTex = new sf::Texture(); // Nécessaire dynamiquement
	if (!tempTex->loadFromFile(_path))
	{
		// Affichage d'un message d'erreur si le chargement de la texture échoue
		std::cerr << "Erreur : Chargement de la texture depuis le fichier '" << _path << "' a échoué." << std::endl;

		// Retourne un sprite vide (ou une autre indication d'erreur)
		return sf::Sprite(); // Sprite vide
	}

	// Création et configuration du sprite
	sf::Sprite tempSprite;
	tempSprite.setTexture(*tempTex);

	if (_isCentered) // Centrage du sprite
	{
		sf::Vector2u tempTaille = tempTex->getSize();
		sf::Vector2f tempOrigin = sf::Vector2f(tempTaille.x / 2.f, tempTaille.y / 2.f);
		tempSprite.setOrigin(tempOrigin);
	}

	// Affichage d'un message de succès

	return tempSprite;
}

sf::Sprite* LoadSpritePtr(const std::string _path, bool _isCentered)
{
	// Création et chargement de la texture
	sf::Texture* tempTex = new sf::Texture(); // Nécessaire dynamiquement
	if (!tempTex->loadFromFile(_path))
	{
		// Affichage d'un message d'erreur si le chargement de la texture échoue
		std::cerr << "Erreur : Chargement de la texture depuis le fichier '" << _path << "' a échoué." << std::endl;

		// Retourne un sprite vide (ou une autre indication d'erreur)
		return nullptr; // Sprite vide
	}

	// Création et configuration du sprite
	sf::Sprite* tempSprite = new sf::Sprite();
	tempSprite->setTexture(*tempTex);

	if (_isCentered) // Centrage du sprite
	{
		sf::Vector2u tempTaille = tempTex->getSize();
		sf::Vector2f tempOrigin = sf::Vector2f(tempTaille.x / 2.f, tempTaille.y / 2.f);
		tempSprite->setOrigin(tempOrigin);
	}

	// Affichage d'un message de succès

	return tempSprite;
}


b2Vec2 Screen2World(sf::Vector2f _screenPos)
{
	b2Vec2 worldPos = { _screenPos.x, _screenPos.y };


	worldPos.y = -worldPos.y;
	worldPos.x /= ZOOM;
	worldPos.y /= ZOOM;
	
	return worldPos;
}

void BlitSprite(sf::Sprite& _sprite, sf::Vector2f _pos, float _angle, sf::RenderWindow& _window)
{
	_sprite.setRotation(_angle);
	_sprite.setPosition(_pos);
	_window.draw(_sprite);
}

float GetAngle(sf::Vector2f _p1, sf::Vector2f _p2)
{
	return atan2f(_p2.y - _p1.y, _p2.x - _p1.x);
}

sf::Vector2f GetPointWithAngleAndDistance(sf::Vector2f _startPosititon, float _distance, float _angle)
{
	sf::Vector2f point2;
	point2.x = _startPosititon.x + _distance * cos(_angle);
	point2.y = _startPosititon.y + _distance * sin(_angle);

	return point2;
}

b2Vec2 GetPointWithAngleAndDistanceB2(b2Vec2 _startPosititon, float _distance, float _angle)
{
	b2Vec2 point2;
	point2.x = _startPosititon.x + _distance * cos(_angle);
	point2.y = _startPosititon.y + _distance * sin(_angle);

	return point2;
}

float DistanceSquared(sf::Vector2f _a, sf::Vector2f _b)
{
	return ((_b.x - _a.x) * (_b.x - _a.x)) + ((_b.y - _a.y) * (_b.y - _a.y));
}

float DistanceSquared(b2Vec2 _a, b2Vec2 _b)
{
	return ((_b.x - _a.x) * (_b.x - _a.x)) + ((_b.y - _a.y) * (_b.y - _a.y));
}

float Distance(sf::Vector2f _vec)
{
	return sqrt(_vec.x * _vec.x + _vec.y * _vec.y);
}

float Distance(sf::Vector2f _a, sf::Vector2f _b)
{
	float r1 = (_b.x - _a.x) * (_b.x - _a.x);
	float r2 = (_b.y - _a.y) * (_b.y - _a.y);
	return sqrt(r1 + r2);
}

float Distance(b2Vec2 _a, b2Vec2 _b)
{
	float r1 = (_b.x - _a.x) * (_b.x - _a.x);
	float r2 = (_b.y - _a.y) * (_b.y - _a.y);
	return sqrt(r1 + r2);
}

b2Vec2 ScreenToB2World(sf::Vector2f _pos)
{
	return b2Vec2(_pos.x / MtoP, -_pos.y / MtoP);
}

b2Vec2 ScreenToB2World(float _x, float _y)
{
	return b2Vec2(_x / MtoP, -_y / MtoP);
}

sf::Vector2f B2WorldToScreen(b2Vec2 _pos)
{
	return sf::Vector2f(_pos.x * MtoP, -_pos.y * MtoP);
}

sf::Vector2f B2WorldToScreen(float _x, float _y)
{
	return sf::Vector2f(_x * MtoP, -_y * MtoP);
}

sf::Color LerpColor(sf::Color color1, sf::Color color2, float t)
{
	t = std::max(std::min(t, 50.0f), 0.0f);

	float ratio1 = t / 50.0f;
	float ratio2 = 1.0f - ratio1;

	sf::Uint8 red = static_cast<sf::Uint8>(color1.r * ratio1 + color2.r * ratio2);
	sf::Uint8 green = static_cast<sf::Uint8>(color1.g * ratio1 + color2.g * ratio2);
	sf::Uint8 blue = static_cast<sf::Uint8>(color1.b * ratio1 + color2.b * ratio2);

	return sf::Color(red, green, blue);
}
sf::Vector2f Normalize(sf::Vector2f _vec, float _distance)
{
	sf::Vector2f normal = _vec / _distance;
	return normal;
}

sf::Vector2f Normalize(sf::Vector2f _vec)
{
	sf::Vector2f normal = _vec / sqrt(_vec.x * _vec.x + _vec.y * _vec.y);
	return normal;
}

sf::Vector2f Normalize(sf::Vector2f _p1, sf::Vector2f _p2)
{
	sf::Vector2f vec = _p1 - _p2;
	sf::Vector2f normal = vec / sqrt(DistanceSquared(_p1, _p2));
	return normal;
}

std::vector<sf::CircleShape*> DebugPoint::ListDebugPoint;
DebugPoint::DebugPoint(sf::Vector2f _pos, float _radius, sf::Color _color)
{
	ListDebugPoint.push_back(new sf::CircleShape);

	ListDebugPoint.back()->setOrigin(_radius, _radius);
	ListDebugPoint.back()->setFillColor(_color);
	ListDebugPoint.back()->setRadius(_radius);
	ListDebugPoint.back()->setPosition(_pos);
}

DebugPoint::DebugPoint(b2Vec2 _pos, float _radius, sf::Color _color)
	: DebugPoint(sf::Vector2f(_pos.x, _pos.y), _radius, _color) {}

int RuleOfThree(int _valX, int _min, int _max, int _minTarget, int _maxTarget)
{
	return int(floor(_minTarget + ((_valX - _min) * (_maxTarget - _minTarget) / (_max - _min))));
}

float RuleOfThree(float _valX, float _min, float _max, float _minTarget, float _maxTarget)
{
	return (_minTarget + ((_valX - _min) * (_maxTarget - _minTarget) / (_max - _min)));
}