#include "GuestManager.h"
#include "common.h"
#include "ItemManager.h"
#include <random>
#include "TextureManager.h"
#include "Buyer.h"
#include "Seller.h"
#include "CharacterStats.h"

nlohmann::json GuestManager::guestData;
std::hash<std::string> GuestManager::hasher;
std::unordered_map<size_t, Guest*> GuestManager::guests;
std::string GuestManager::path;

// ai used for the text : copy.ai

Guest* GuestManager::GetGuest(std::string guestName)
{
	size_t hashValue = hasher(guestName);
	auto it = guests.find(hashValue);
	if (it != guests.end()) 
	{
		return it->second;
	}

	return nullptr;
}

std::list<Guest*> GuestManager::GetGuestList() 
{
	float xOffset = 0.0f;
	float yOffset = 0.0f;

	std::list<Guest*> guestList;
	for (auto& pair : guests) 
	{
		

		Guest* guest = pair.second;
		guest->SetPos(sf::Vector2f(800 - xOffset, 550 - yOffset));
		yOffset -= 60.0f;
		xOffset -= 100.0f;
		guestList.push_back(guest);
	}
	return guestList;
}

//std::list<Guest*> GuestManager::GetGuestList()
//{
//	std::list<Guest*> guestList;
//	int count = 0; 
//
//	for (auto& pair : guests)
//	{
//		if (count >= 3) 
//			break;
//
//		Guest* guest = pair.second;
//		guest->SetPos(sf::Vector2f(200.f, -500.f));
//		guestList.push_back(guest);
//
//		count++; 
//	}
//
//	return guestList;
//}

//void GuestManager::CreateLine(std::list<Guest*>* _guests, sf::Vector2f _finalPos)
//{
//	// décalage selon pos dans la liste
//	float xOffset = 0.0f;
//	float yOffset = 0.0f;
//	for (auto guest : *_guests)
//	{
//		guest->Update(/*sf::Vector2f(_finalPos.x, _finalPos.y), xOffset, yOffset*/);
//		yOffset -= 60.0f;
//		xOffset -= 100.0f;
//	}
//}



std::list<Guest*> GuestManager::GetRandomGests()
{
	std::list<Guest*> randomGuests;

	std::vector<size_t> availableGuestIDs;

	for ( auto& pair : guests)
	{
		availableGuestIDs.push_back(pair.first);
	}

	// pour shuffle la list, 5 max pour test
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(availableGuestIDs.begin(), availableGuestIDs.end(), g);

	// Récupérer les 5 premiers 
	size_t count = 0;
	for (size_t id : availableGuestIDs)
	{
		if (count >= 2)
			break;

		Guest* guest = GuestManager::guests[id];
		randomGuests.push_back(guest);
		++count;
	}

	for (Guest* guest : randomGuests)
	{
		randomGuests.push_back(guest);
	}

	return randomGuests;

}
// Read the json file to create all guest accordate to the data
void GuestManager::Init(std::string _jsonPath)
{
    //std::srand((unsigned)std::time(nullptr)); //Deja dans le main
    path = _jsonPath;
    std::ifstream file(path);
    if (file.is_open())
    {
        file >> guestData;
    }
    file.close();
    guests.clear();
    for (auto i = guestData.begin(); i != guestData.end(); i++)
    {
        Guest* tempGuest = nullptr;
		int random_int =  std::rand() % 2 + 1;
		//random_int = 1;
        if (random_int == 1)
        {
            tempGuest = new Seller();
        }
        else
        {
            tempGuest = new Buyer();
        }
		
        tempGuest->SetName(i.key());
        tempGuest->SetBubbleSprite(new sf::Sprite(LoadSprite("Assets/Sprites/Shop/Guests/bubble.png", true)));
        tempGuest->SetInteractSprite(new sf::Sprite(LoadSprite("Assets/Sprites/Shop/Guests/button.png", true)));
        auto& guestTypeData = i.value(); 

		auto& sellerData = guestTypeData["Seller"];
		auto& buyerData = guestTypeData["Buyer"];

		if (random_int == 1) 
		{
			if (tempGuest->GetName() == "Chester")
			{
				if (GameData::CharacterStats->GetWalkBoostLevel() == 0)
				{
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item"]));
				}
				else if (GameData::CharacterStats->GetWalkBoostLevel() == 1)
				{
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item2"]));
				}
				else if (GameData::CharacterStats->GetWalkBoostLevel() == 2)
				{
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item3"]));
				}
				else
				{
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item4"]));
				}
			}
			else if (tempGuest->GetName() == "Barley")
			{
				if (GameData::CharacterStats->GetDamageBoostLevel() == 0)
				{
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item"]));
				}
				else if (GameData::CharacterStats->GetDamageBoostLevel() == 1)
				{
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item2"]));
				}
				else if (GameData::CharacterStats->GetDamageBoostLevel() == 2)
				{
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item3"]));
				}
				else
				{
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item4"]));
				}
			}
			else
			{
				if (GameData::CharacterStats->CanUseSpecialSpell() == false)
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item"]));
				else
					tempGuest->SetItem(ItemManager::GetItem(sellerData["Item2"]));
			}

			sf::Sprite* tempSprite = new sf::Sprite(LoadSprite(sellerData["SpritePath"], true));
			tempGuest->SetSprite(tempSprite);

			tempGuest->SetItemQuantity(sellerData["ItemQuantity"]);
			tempGuest->SetPrice(sellerData["Price"] * tempGuest->GetItemQuantity());		
			tempGuest->SetSentence(sellerData["Sentence"]);
			tempGuest->SetThanksSentence(sellerData["ThanksSentence"]);
			tempGuest->SetDenySentence(sellerData["DenySentence"]);
		}
		else 
		{
			tempGuest->SetItem(ItemManager::GetItem(buyerData["Item"]));
			tempGuest->SetItemQuantity(buyerData["ItemQuantity"]);

			tempGuest->SetPrice(buyerData["Price"] * tempGuest->GetItemQuantity());

			sf::Sprite* tempSprite = new sf::Sprite(LoadSprite(buyerData["SpritePath"], true));
			tempGuest->SetSprite(tempSprite);

			//std::cout << tempGuest->GetPrice() << std::endl;
			tempGuest->SetSentence(buyerData["Sentence"]);
			tempGuest->SetThanksSentence(buyerData["ThanksSentence"]);
			tempGuest->SetDenySentence(buyerData["DenySentence"]);
		}
        // Set common data outside if-else
        tempGuest->SetId(guestTypeData["Id"]);
	
		std::string modifiedSentence = tempGuest->GetSentence();
		Dialog* dialog = new Dialog();
		dialog->text = modifiedSentence;
		dialog->name = tempGuest->GetName();
		dialog->instant = false;
		dialog->speed = 25.f;
		tempGuest->SetDialog(dialog);
        size_t hashValue = hasher(i.key());
        guests.insert({ hashValue, tempGuest });
    }
    std::cerr << "Guests initialisation complete with " << guests.size() << " Guests" << std::endl;
}

std::string GuestManager::InsertNewlineInString(const std::string& _string)
{
	int middle = (int)_string.size() / 2;

	int spacePos = (int)_string.rfind(' ', middle);

	if (spacePos == std::string::npos || spacePos < middle / 2 || spacePos > middle + middle / 2) 
	{
		spacePos = middle;
	}

	std::string modifiedStr = _string.substr(0, spacePos) + "\n" + _string.substr(spacePos);

	return modifiedStr;
}