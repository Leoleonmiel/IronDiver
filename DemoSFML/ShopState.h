#ifndef SHOP_STATE__H
#define SHOP_STATE__H

#include <list>
#include "GameState.h"
#include "Slot.h"

//class CameraManager;
//class TiledManager;
class CharaControl;
class ShopInventory;
class Guest;
class GUIManager;
//namespace b2 { class Rectangle; } 

class ShopState : public GameState
{
public:
	ShopState() {}
	void Init() override;
	void Update(float) override;
	void Quit() override;
	void Display(sf::RenderWindow&) override;
	void HandleEvents(sf::Event&) override;
	
private:
	//CameraManager* m_camera = nullptr;
	ShopInventory* m_shopInventory = nullptr;
	std::list<Guest*> m_guests;
	sf::Sprite* m_dropSprite = nullptr;
	sf::Text* m_goldAmountLabel = nullptr;
	sf::Text* m_priceLabel = nullptr;
	sf::Font* m_font = nullptr;
	GUIManager* m_guiManager = nullptr;
	float m_elapsedTime = 0.f;
	Slot* m_itemSlot = nullptr;
	float m_nextGuestTimer = 0.f;
	bool m_isGamePaused = false;
	float m_lastGameSpeed = 0;
	float m_elapsedTimeShake = 0.f;
	//TiledManager* m_tiledManager = nullptr;
};
#endif
