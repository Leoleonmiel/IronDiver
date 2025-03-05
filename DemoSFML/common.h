#ifndef COMMON__H
#define COMMON__H

#include <iostream>
#include <bitset>
#include <list>
#include <cstdint>

#include "SFML/Graphics.hpp"
////#include "SFML/OpenGL.hpp" //CRASH
#include <box2d/box2d.h>

////////////////////////////////////////////////////////////////////
//AUCUN INCLUDES PERSO NE DOIVENT ETRE A L'INTERIEUR DE CE FICHIER//
////////////////////////////////////////////////////////////////////

#define MtoP 16.f //One meter in pixels, Affect mass of Box2D objects
#define ZOOM MtoP //Affect the size of Box2D display Shapes, probably useless in our case
#define NONE -1

#define SCREEN_SIZE sf::Vector2f{1920, 1080}
#define GRAVITY 98.7f
#define GRAVITY_BOX2D sf::Vector2f(0.f,-98.7f)

#define _PI 3.1415926f
#define MAX_FPS 60

namespace b2 { class World; }
class CharaControl;
class Collectable;
class PlayerInventory;
class Cursor;
class CharacterStats;

enum Direction
{
	NO_DIR = -1,

	LEFT,
	RIGHT,
	UP,
	DOWN
};

enum FilterType
{
	F_PLAYER = (uint16_t)0b0000000000000001,
	F_MONSTER = (uint16_t)0b0000000000000010,

	F_HURTBOX = (uint16_t)0b00000000010000
};

enum GroupType
{
	G_PLAYER = -1,
	G_MAP = -2
};

struct GameData
{
	static sf::Clock Clock;
	static sf::Time ClockTime;
	static sf::RenderWindow* Window;
	static b2::World* World;
	static float DeltaTime;
	static float GameTime;
	static float GameSpeed;
	static CharaControl* CharaCtrl;
	static std::list<Collectable*>* Collectables;
	static Cursor* Cursor;
	static CharacterStats* CharacterStats;
	static bool fullscreen;
	static sf::Vector2f GetMousePos()
	{
		return sf::Vector2f(sf::Mouse::getPosition(*Window));
	};
	static sf::Vector2f GetRelativeMousePos()
	{
		return sf::Vector2f(Window->mapPixelToCoords(sf::Mouse::getPosition(*Window)));
	}
};

sf::Sprite LoadSprite(const std::string _path, bool _isCentered);
sf::Sprite* LoadSpritePtr(const std::string _path, bool _isCentered);
void BlitSprite(sf::Sprite& _sprite, sf::Vector2f _pos, float _angle, sf::RenderWindow& _window);
sf::Vector2f World2Screen(b2Vec2 _worldPos, sf::Vector2f _cam);
b2Vec2 Screen2World(sf::Vector2f _screenPos);
sf::Color LerpColor(sf::Color a, sf::Color b, float t);

float GetAngle(sf::Vector2f _p1, sf::Vector2f _p2);
sf::Vector2f GetPointWithAngleAndDistance(sf::Vector2f _startPosititon, float _distance, float _angle);
b2Vec2 GetPointWithAngleAndDistanceB2(b2Vec2 _startPosititon, float _distance, float _angle);
float DistanceSquared(sf::Vector2f _a, sf::Vector2f _b);
float DistanceSquared(b2Vec2 _a, b2Vec2 _b = b2Vec2{ 0,0 });

float Distance(sf::Vector2f _vec);
float Distance(sf::Vector2f _a, sf::Vector2f _b);
float Distance(b2Vec2 _a, b2Vec2 _b);

b2Vec2 ScreenToB2World(sf::Vector2f _pos);
b2Vec2 ScreenToB2World(float _x, float _y);

sf::Vector2f B2WorldToScreen(b2Vec2 _pos);
sf::Vector2f B2WorldToScreen(float _x, float _y);

sf::Vector2f Normalize(sf::Vector2f _vec, float _distance);
sf::Vector2f Normalize(sf::Vector2f _vec);
sf::Vector2f Normalize(sf::Vector2f _p1, sf::Vector2f _p2);

int RuleOfThree(int _valX, int _min, int _max, int _minTarget, int _maxTarget);
float RuleOfThree(float _valX, float _min, float _max, float _minTarget, float _maxTarget);

struct DebugPoint
{
	DebugPoint(sf::Vector2f _pos, float _radius = 4.f, sf::Color _color = sf::Color::Red);
	DebugPoint(b2Vec2 _pos, float _radius = 4.f, sf::Color _color = sf::Color::Red);

	static std::vector<sf::CircleShape*> ListDebugPoint;
};



#endif