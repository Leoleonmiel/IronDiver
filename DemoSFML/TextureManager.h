#pragma once

#include <unordered_map>
#include <string>

#include "SFML/Graphics.hpp"

class TextureManager
{
	static std::unordered_map<std::string, sf::Texture*> m_textureAtlas;

public:
	static sf::Texture* GetTexture(std::string);

	static void LoadTextures(std::string);
	static void UnloadTextures(std::string);
	static void ClearTextures();

	static sf::Sprite* CreateSprite(std::string _name, bool _isCentered = false);
	static sf::Sprite* CreateSpriteFoot(const std::string _name);

	static std::unordered_map<std::string, sf::Texture*>* GetTextureAtlas();
	static void BlitSprite(sf::Sprite& _sprite, sf::Vector2f _pos, float _angle, sf::RenderWindow& _window);
};

