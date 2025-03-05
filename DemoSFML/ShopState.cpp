#include "Box2D_World.h"
#include "Collectable.h"
#include "common.h"
#include "ItemManager.h"
#include "ShopState.h"
//#include "TiledManager.hpp"
#include "GuestManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "Inventory.h"
#include "CraftManager.h"
#include "GUIManager.h"
#include "GUI.h"
#include "TextureManager.h"
//#include "ItemManager.h"
#include "ScrollManager.h"
#include "SoundManager.h"
#include "Seller.h"
#include "LoadingState.h"
#include "FadeoutState.h"
#include "FadeinState.h"
#include "MineState.h"
#include "MainMenu.h"
#include "CharacterController.h"
#include "CharacterStats.h"
#include "Cursor.h"
#include <random>

void ShopState::Init()
{
	SoundManager::GetInstance()->StopAll();
	TextureManager::LoadTextures("Assets/Sprites");
	ItemManager::Init("ConfigFiles/Data/ItemData.json");
	GUIImage::TextureSearchFunction = &TextureManager::GetTexture;
	GUIText::Font = new sf::Font;
	GUIText::Font->loadFromFile("Assets/Fonts/Orbitron-ExtraBold.ttf");

	GuestManager::Init("ConfigFiles/Guests/GuestData.json");
	m_guests = GuestManager::GetGuestList();
	m_itemSlot = new Slot("Assets/Sprites/Shop/drop.png", sf::Vector2f(1145, 780), m_guests.front()->GetItem());
	m_guiManager = GUIManager::GetInstance();

	std::ifstream dialogsFile("ConfigFiles/UI/ShopUI.dat");
	if (dialogsFile.is_open())
	{
		m_guiManager->Load(dialogsFile);
		dialogsFile.close();
	}

	DialogMaster::InitialiseDialogMaster();
	DialogMaster::SetEnabled(true);

	GUI* dialogContainer = (GUI*)GUIManager::GetInstance()->GetNodeByTag("DialogContainer");
	GUI* tradeContainer = (GUI*)GUIManager::GetInstance()->GetNodeByTag("TradeContainer");
	GUIImage* nextButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("NextClient");
	GUIImage* exploreContainer = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("ExploreContainer");
	GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");




	exploreContainer->SetEnabled(false);
	/*nextButton->SetEnabled(false);*/
	dialogContainer->SetEnabled(false);
	tradeContainer->SetEnabled(false);
	pauseGUI->SetEnabled(false);

	CraftManager::GetInstance()->Init();

	Inventory::shopInventory->InitInventoryDisplay();

	SoundManager::GetInstance()->ChangeState(DIALOGUE);
	SoundManager::GetInstance()->Play("ShopTheme");

	GameData::Cursor->SetIsVisible(true);


}

void ShopState::Quit()
{
	CraftManager::FreeMemory();
	//delete m_tiledManager;

	for (auto& guest : m_guests)
	{
		delete guest;
	}
	m_guests.clear();
	GameData::Cursor->SetIsVisible(false);

}

void ShopState::Update(float _dt)
{
	////////////////////////////////////////////////
	//don't place any functions before world update
	//GameData::World->Update();
	InputManager& inputs = *InputManager::GetInstance();
	ScrollManager::GetInstance()->Update();
	SoundManager::GetInstance()->Update();
	CraftManager::GetInstance()->Update();

	sf::Vector2i mousePos = (sf::Vector2i)GameData::GetMousePos();

	GUIManager::GetInstance()->GetNodeByTag("craftButton")->Update(GameData::DeltaTime, *GameData::Window);

	GUI* dialogContainer = (GUI*)GUIManager::GetInstance()->GetNodeByTag("DialogContainer");
	GUIImage* alexSprite = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("CharacterSprite");
	GUI* tradeContainer = (GUI*)GUIManager::GetInstance()->GetNodeByTag("TradeContainer");
	GUI* sellContainer = (GUI*)GUIManager::GetInstance()->GetNodeByTag("SellContainer");
	GUI* buyContainer = (GUI*)GUIManager::GetInstance()->GetNodeByTag("BuyContainer");
	GUIText* nameText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("NameText");
	GUIText* dialogText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("DialogText");
	GUIImage* exploreContainer = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("ExploreContainer");
	GUIImage* inventoryButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("InventoryButton");
	GUIImage* inventoryCloseButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("InventoryCloseButton");
	GUIImage* nextClient = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("NextClient");
	GUIImage* exploreButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("ButtonContainer");

	m_itemSlot->Update(_dt);

	

	// can escape at any moment
	if (inputs.IsSinglePress("Escape"))
	{
		if (!m_isGamePaused)
		{
			m_isGamePaused = true;
			m_lastGameSpeed = GameData::GameSpeed;
			GameData::GameSpeed = 0;
			GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");
			pauseGUI->SetEnabled(true);
		}
		else
		{
			m_isGamePaused = false;
			GameData::GameSpeed = m_lastGameSpeed;
			GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");
			pauseGUI->SetEnabled(false);
		}
	}

	// when game is paused

	if (m_isGamePaused)
	{
		sf::Vector2i mousePos = (sf::Vector2i)GameData::GetMousePos();
		GUI* playButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PlayButton");
		GUI* backMenuButton = (GUI*)GUIManager::GetInstance()->GetNodeByTag("BackMenuButton");


		// return in game
		if (GUI* guiPointer = dynamic_cast<GUI*>(playButton))
		{
			if (playButton->DetectClick(mousePos, guiPointer))
			{
				if (inputs.IsSinglePress("Attack"))
				{
					m_isGamePaused = false;
					GameData::GameSpeed = m_lastGameSpeed;
					GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");
					pauseGUI->SetEnabled(false);
				}
			}
		}


		// go to main menu
		if (GUI* guiPointer = dynamic_cast<GUI*>(backMenuButton))
		{
			if (backMenuButton->DetectClick(mousePos, guiPointer))
			{
				if (inputs.IsSinglePress("Attack"))
				{
					m_isGamePaused = false;
					GameData::GameSpeed = m_lastGameSpeed;
					GUI* pauseGUI = (GUI*)GUIManager::GetInstance()->GetNodeByTag("PauseState");
					pauseGUI->SetEnabled(false);

					LoadingState::StartLoading<MainMenu>();
					PushState(new FadeOutState(new LoadingState()));
				}
			}
		}
	}

	//go to mine 
	if (GUI* guiPointer = dynamic_cast<GUI*>(exploreButton))
	{
		if (exploreButton->DetectClick(mousePos, guiPointer))
		{
			if (inputs.IsSinglePress("Attack") && exploreContainer->IsEnabled())
			{

				LoadingState::StartLoading<MineState>();
				PushState(new FadeOutState(new LoadingState()));
			}
		}
	}
	//close inventory
	if (GUI* guiPointer = dynamic_cast<GUI*>(inventoryCloseButton))
	{
		if (inventoryCloseButton->DetectClick(mousePos, guiPointer))
		{
			if (inputs.IsSinglePress("Attack"))
			{

				Inventory::shopInventory->showingInventory = false;
			}
		}
	}

	//show inventory
	if (GUI* guiPointer = dynamic_cast<GUI*>(inventoryButton))
	{
		if (inventoryButton->DetectClick(mousePos, guiPointer) && inventoryButton->IsEnabled())
		{
			if (inputs.IsSinglePress("Attack"))
			{
				Inventory::shopInventory->showingInventory = !Inventory::shopInventory->showingInventory;
			}
		}
	}

	//actions when there is at least 1 client

	if (!m_guests.empty())
	{
		Guest* firstGuest = m_guests.front();
		Inventory::shopInventory->Update(m_itemSlot, firstGuest->GetItem());

		if (firstGuest != nullptr && firstGuest->GetState() != Guest::BUSY)
		{
			Seller* sellerGuest = dynamic_cast<Seller*>(firstGuest);
			if (sellerGuest != nullptr)
			{
				m_itemSlot->SetItem(firstGuest->GetItem());
			}
			m_itemSlot->SetItemNeeded(firstGuest->GetItem());
			firstGuest->SetState(Guest::READY_TO_INTERACT);

		}


		if (firstGuest->CanInteract())
		{
			GUIImage* guestSprite = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("GuestSprite");
			guestSprite->ChangeTexture(*firstGuest->GetSprite()->getTexture());
			dialogContainer->SetEnabled(true);
			nameText->SetText(firstGuest->GetName());
			firstGuest->SetState(Guest::BUSY);
			DialogMaster::TrySetDialog(*firstGuest->GetDialog());

		}

		// when slot item has an item and guest can trade
		if (firstGuest->CanTrade() && m_itemSlot->GetItem() != nullptr)
		{
			GUIImage* sellButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("SellButton");
			GUIImage* cancelButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("CancelButton");

			if (GUI* guiPointer = dynamic_cast<GUI*>(sellButton))
			{
				if (sellButton->DetectClick(mousePos, guiPointer) && sellButton->IsEnabled())
				{
					if (inputs.IsSinglePress("Attack"))
					{
						tradeContainer->SetEnabled(false);
						sellContainer->SetEnabled(false);
						buyContainer->SetEnabled(false);
						SoundManager::GetInstance()->Play("Sell");

						Dialog* dialog = new Dialog();
						dialog->text = firstGuest->GetThanksSentence();
						dialog->name = firstGuest->GetName();
						dialog->instant = false;
						dialog->speed = 50.f;
						firstGuest->SetDialog(dialog);
						DialogMaster::TrySetDialog(*firstGuest->GetDialog());

						Seller* sellerGuest = dynamic_cast<Seller*>(firstGuest);
						if (sellerGuest == nullptr)
						{
							Inventory::shopInventory->AddGold(firstGuest->GetPrice());
							if (m_itemSlot->GetItem() != nullptr) Inventory::shopInventory->RemoveItem(m_itemSlot->GetItem(), 1);
						}
						else
						{
							Inventory::shopInventory->RemoveGold(firstGuest->GetPrice());
						
							bool canAdd = false;

							if (m_itemSlot->GetItem()->name == "UpgradeAttack1")
							{
								GameData::CharacterStats->SetDamageBoostLevel(1);
							}
							else if (m_itemSlot->GetItem()->name == "UpgradeAttack1")
							{
								GameData::CharacterStats->SetDamageBoostLevel(2);
							}
							else if (m_itemSlot->GetItem()->name == "UpgradeAttack3")
							{
								GameData::CharacterStats->SetDamageBoostLevel(3);
							}
							else if (m_itemSlot->GetItem()->name == "UpgradeSpeed1")
							{
								GameData::CharacterStats->SetWalkBoostLevel(1);
							}
							else if (m_itemSlot->GetItem()->name == "UpgradeSpeed2")
							{
								GameData::CharacterStats->SetWalkBoostLevel(2);
							}
							else if (m_itemSlot->GetItem()->name == "UpgradeSpeed3")
							{
								GameData::CharacterStats->SetWalkBoostLevel(3);
							}
							else if (m_itemSlot->GetItem()->name == "UpgradeAttackHaut")
							{
								GameData::CharacterStats->SetCanUseSpecialSpell(true);
							}
							else
								canAdd = true;

							if (canAdd)
							{
								Inventory::shopInventory->AddItem(m_itemSlot->GetItem(), 1);
							}
								

						}
						m_itemSlot->SetItemNeeded(nullptr);
						m_itemSlot->SetItem(nullptr);
					
					}
				}
			}

			if (GUI* guiPointer = dynamic_cast<GUI*>(cancelButton))
			{
				if (cancelButton->DetectClick(mousePos, guiPointer))
				{
					if (inputs.IsSinglePress("Attack"))
					{
						Dialog* dialog = new Dialog();
						dialog->text = firstGuest->GetDenySentence();
						dialog->name = firstGuest->GetName();
						dialog->instant = false;
						dialog->speed = 50.f;
						firstGuest->SetDialog(dialog);
						DialogMaster::TrySetDialog(*firstGuest->GetDialog());

						/*m_guests.pop_front();*/
						Inventory::shopInventory->showingInventory = !Inventory::shopInventory->showingInventory;
						m_itemSlot->SetItemNeeded(nullptr);
						m_itemSlot->SetItem(nullptr);
						tradeContainer->SetEnabled(false);
						sellContainer->SetEnabled(false);
						buyContainer->SetEnabled(false);
					}
				}
			}
		}

		if (!DialogMaster::GetDialogRunning() && m_itemSlot->GetItemNeeded() == nullptr)
		{
			m_nextGuestTimer += GameData::DeltaTime;
		
		}

		if (m_nextGuestTimer > 5.0f)
		{
		
			delete m_guests.front();
			m_guests.front() = nullptr;
			m_guests.pop_front();
			m_nextGuestTimer = 0.0f;
		}

		//show trade container when drag and drop item
		if (m_itemSlot->GetItem() != nullptr)
		{
			Seller* sellerGuest = dynamic_cast<Seller*>(firstGuest);
			if (sellerGuest != nullptr)
			{
				GUIImage* itemToSell = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("ItemToBuy");
				GUIText* price = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("BuyPrice");
				GUIText* itemName = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("ItemBuyName");
				itemName->SetText(m_itemSlot->GetItem()->name);
				price->SetText(std::to_string(firstGuest->GetPrice()) + "$");
				tradeContainer->SetEnabled(true);
				sellContainer->SetEnabled(false);
				buyContainer->SetEnabled(true);
				itemToSell->ChangeTexture(*m_itemSlot->GetItem()->_slotSprite->getTexture());
			}
			else
			{


				GUIImage* itemToSell = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("ItemToSell");
				GUIText* price = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("SellPrice");
				GUIText* itemName = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("ItemSellName");
				itemName->SetText(m_itemSlot->GetItem()->name);
				if (firstGuest) price->SetText(std::to_string(firstGuest->GetPrice()) + "$");
				tradeContainer->SetEnabled(true);
				sellContainer->SetEnabled(true);
				itemToSell->ChangeTexture(*m_itemSlot->GetItem()->_slotSprite->getTexture());
				m_itemSlot->SetItem(firstGuest->GetItem());

			}
		}

	}
	else
	{
		GameData::CharacterStats->LoadStatsJSON("ConfigFiles/Character/CharacterData.json");
		GameData::CharacterStats->LoadBoostJSON("ConfigFiles/Character/CharacterBoostData.json");

		GUIText* speedText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("SpeedText");
		GUIText* damageSpell = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("DamageText");
		GUIText* spellText = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("SpellText");

		speedText->SetText(std::to_string((int)GameData::CharacterStats->GetSpeed()));
		damageSpell->SetText(std::to_string((int)GameData::CharacterStats->GetDamage()));
		spellText->SetText(std::to_string(GameData::CharacterStats->CanUseSpecialSpell()));

		exploreContainer->SetEnabled(true);
		dialogContainer->SetEnabled(false);

		m_itemSlot = nullptr;
		Inventory::shopInventory->showingInventory = false;
	}


	/*if (!alexSprite->IsEnabled())
	{
		alexSprite->SetEnabled(true);
	}*/


	// Give items for milestone
	if (inputs.IsSinglePress("Jump"))
	{
		GameData::CharacterStats->LoadStatsJSON("ConfigFiles/Character/CharacterData.json");
		GameData::CharacterStats->LoadBoostJSON("ConfigFiles/Character/CharacterBoostData.json");
		Inventory::shopInventory->AddItem(ItemManager::GetItem("Aurorium"), 200);
		Inventory::shopInventory->AddItem(ItemManager::GetItem("Etherium"), 200);
		Inventory::shopInventory->AddItem(ItemManager::GetItem("Rock"), 200);
		Inventory::shopInventory->AddItem(ItemManager::GetItem("Chromatica"), 200);
	}
	if (GUI* guiPointer = dynamic_cast<GUI*>(nextClient))
	{
		if (nextClient->DetectClick(mousePos, guiPointer))
		{
			if (inputs.IsSinglePress("Attack"))
			{
				m_guests.pop_front();
				m_itemSlot->SetItem(nullptr);
				tradeContainer->SetEnabled(false);
			}
		}
	}
}



void ShopState::Display(sf::RenderWindow& _window)
{
	sf::View view = _window.getView();
	_window.setView(_window.getDefaultView());

	DialogMaster::UpdateDialog();

	GUIImage* bckSellerName = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("DialogNameBackgroundSeller");
	GUIImage* bckBuyerName = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("DialogNameBackgroundBuyer");
	GUIImage* nextButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("NextClient");

	if (m_guiManager != nullptr && m_guiManager->GetCurrentRoot() != nullptr)
	{
		sf::View view = _window.getView();
		_window.setView(_window.getDefaultView());
		m_guiManager->GetCurrentRoot()->Display(_window);
		_window.setView(view);
	}

	

	if (!m_guests.empty())
	{
		int count = 0;
		/*	for (auto& guest : m_guests)
			{
				guest->Display();
			}*/
		if (m_guests.front()->GetState() == Guest::BUSY)
		{
			Seller* sellerGuest = dynamic_cast<Seller*>(m_guests.front());
			if (sellerGuest != nullptr)
			{
				bckSellerName->SetEnabled(true);
				bckBuyerName->SetEnabled(false);
			}
			else
			{
				bckSellerName->SetEnabled(false);
				bckBuyerName->SetEnabled(true);
				nextButton->SetEnabled(true);
			}

			nextButton->SetEnabled(true);
		}
		else
			nextButton->SetEnabled(false);
	}
	else
	{
		nextButton->SetEnabled(false);
	}

	if (Inventory::shopInventory->showingInventory)
	{
		m_itemSlot->Display(_window);
		GUIImage* closeInventoryButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("InventoryCloseButton");
		closeInventoryButton->SetEnabled(true);
		GUIImage* inventoryButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("InventoryButton");
		inventoryButton->SetEnabled(false);
		CraftManager::GetInstance()->Display(_window);
		Inventory::shopInventory->ShowInventory(_window, m_itemSlot);
	}
	else
	{
		GUIImage* inventoryButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("InventoryButton");
		inventoryButton->SetEnabled(true);
		GUIImage* closeInventoryButton = (GUIImage*)GUIManager::GetInstance()->GetNodeByTag("InventoryCloseButton");
		closeInventoryButton->SetEnabled(false);
	}

	_window.setView(view);
}

void ShopState::HandleEvents(sf::Event& _event)
{
	if (_event.type == sf::Event::KeyPressed)
	{
	}

}
