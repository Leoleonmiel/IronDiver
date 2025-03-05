#include "TextureManager.h"
#include <iostream>
#include <filesystem>

std::unordered_map<std::string, sf::Texture*> TextureManager::m_textureAtlas;

sf::Texture* TextureManager::GetTexture(std::string _name)
{
	auto result = m_textureAtlas.find(_name);
	if (result != m_textureAtlas.end())
	{
		return (*result).second;
	}

	return nullptr;
}

sf::Sprite* TextureManager::CreateSprite(std::string _name, bool _isCentered)
{
	auto result = m_textureAtlas.find(_name);
	if (result != m_textureAtlas.end())
	{
		sf::Sprite* newSprite = new sf::Sprite;
		newSprite->setTexture(*result->second);

		if (_isCentered)
		{
			sf::Vector2u tempTaille = result->second->getSize();
			sf::Vector2f tempOrigin = sf::Vector2f(tempTaille.x / 2.f, tempTaille.y / 2.f);
			newSprite->setOrigin(tempOrigin);
		}
		return newSprite;
	}
	std::cout << "Texture \"" << _name << "\" not found" << std::endl;
	return nullptr;
}

sf::Sprite* TextureManager::CreateSpriteFoot(const std::string _name)
{
	auto result = m_textureAtlas.find(_name);
	if (result != m_textureAtlas.end())
	{
		sf::Sprite* newSprite = new sf::Sprite;
		newSprite->setTexture(*result->second);

		sf::Vector2u tempTaille = result->second->getSize();
		sf::Vector2f tempOrigin = sf::Vector2f(tempTaille.x / 2.f, static_cast<float>(tempTaille.y));
		newSprite->setOrigin(tempOrigin);

		return newSprite;
	}
	std::cout << "Texture \"" << _name << "\" not found" << std::endl;
	return nullptr;
}

void TextureManager::LoadTextures(std::string _path)
{
	sf::Image image;
	sf::Texture text;
	std::string dynamicPath;
	std::string extension;
	std::string textureName;

	for (auto& file : std::filesystem::directory_iterator(_path))
	{
		dynamicPath = file.path().generic_string();

		if (file.is_directory())
		{
			LoadTextures(dynamicPath);
		}
		else
		{
			extension = file.path().extension().generic_string();

			if (extension == ".png" || extension == ".bmp")
			{
				textureName = file.path().filename().generic_string();
				if (m_textureAtlas.find(textureName) == m_textureAtlas.end())
				{
					image.loadFromFile(dynamicPath);
					text.loadFromImage(image);
					m_textureAtlas[textureName] = new sf::Texture(text);
				}
			}
		}
	}
}

void TextureManager::UnloadTextures(std::string _path)
{
	std::string dynamicPath;
	std::string extension;
	std::string textureName;

	for (auto& file : std::filesystem::directory_iterator(_path))
	{
		dynamicPath = file.path().generic_string();

		if (file.is_directory())
		{
			UnloadTextures(dynamicPath);
		}
		else
		{
			extension = file.path().extension().generic_string();

			if (extension == ".png" || extension == ".bmp")
			{
				textureName = file.path().filename().generic_string();
				if (m_textureAtlas.find(textureName) != m_textureAtlas.end())
				{
					delete m_textureAtlas[textureName];
					m_textureAtlas.erase(textureName);
				}
			}
		}
	}
}

void TextureManager::ClearTextures()
{
	m_textureAtlas.clear();
}

std::unordered_map<std::string, sf::Texture*>* TextureManager::GetTextureAtlas()
{
	return &m_textureAtlas;
}

void TextureManager::BlitSprite(sf::Sprite& _sprite, sf::Vector2f _pos, float _angle, sf::RenderWindow& _window)
{
	_sprite.setRotation(_angle);
	_sprite.setPosition(_pos);
	_window.draw(_sprite);
}