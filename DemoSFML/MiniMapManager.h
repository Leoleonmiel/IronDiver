#ifndef MINIMAPMANAGER_H
#define MINIMAPMANAGER_H

#include "SFML/Graphics.hpp"
#include <vector>

#define OFFSET sf::Vector2f(12.f, 212.f)

class MiniMapManager
{
public:
	~MiniMapManager();
	static MiniMapManager* GetInstance();
	void Init();
	void Update();
	void Display(sf::RenderWindow& _window);
	void SetFullMap(bool _value);
	bool IsFullMap() { return m_isFullMap; }
	static void FreeMemory();
private:
	static MiniMapManager* Instance;
	sf::View *m_mapView = nullptr;
	sf::Sprite* m_playerSprite = nullptr;
	sf::Sprite* m_mapSprite = nullptr;
	sf::Sprite* m_mapBackground = nullptr;
	bool m_isFullMap = false;
	sf::Vector2f m_oldMousePos;
};


#endif // !

