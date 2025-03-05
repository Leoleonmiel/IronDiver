#ifndef GUESTMANAGER_H
#define GUESTMANAGER_H

#include "SFML/Graphics.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "Guest.h"

class GuestManager
{
public:
    static void Init(std::string);
    //static void Save();
    //static void AddGuest(Guest);
    static Guest* GetGuest(std::string);
    static std::list<Guest*> GetRandomGests();
    static std::list<Guest*> GetGuestList();
    static void CreateLine(std::list<Guest*>*, sf::Vector2f);
    static std::string InsertNewlineInString(const std::string&);
    //static void Remove(std::list<Guest*>*, sf::Vector2f);
private:
    static std::list<Guest*> items;
    static nlohmann::json guestData;
    static std::string path;
    static std::hash<std::string> hasher;
    static std::unordered_map<size_t, Guest*> guests;
    
};

#endif // !GUESTMANAGER_H
