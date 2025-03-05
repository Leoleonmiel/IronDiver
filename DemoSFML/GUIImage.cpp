#include "GUI.h"
#include <fstream>

TextureSearch GUIImage::TextureSearchFunction;

void GUIImage::SaveBaseData(std::ofstream& _stream, GUIManager& _manager)
{
	GUI::SaveBaseData(_stream, _manager);

	if (m_textureName == "")
	{
		m_textureName = "void";
	}

	_stream << "Image " << m_textureName << std::endl;
}

void GUIImage::LoadCommand(std::string _command, std::ifstream& _stream, GUIManager& _manager, int _baseParent)
{
	std::string inputStr;

	inputStr = _command;

	if (inputStr == "Image")
	{
		_stream >> inputStr;
		ChangeTexture(inputStr);
		return;
	}
	GUI::LoadCommand(_command, _stream, _manager, _baseParent);
}

void GUIImage::ChangeTexture(std::string _name)
{
	m_textureName = _name;
	sf::Texture* texture = TextureSearchFunction(_name);
	if (texture == nullptr)
	{
		m_texture = sf::Texture();
	}
	else
	{
		m_texture = *TextureSearchFunction(_name);
	}
	
	m_shape.setTexture(&m_texture);
	m_shape.setTextureRect({ 0,0, (int)m_texture.getSize().x, (int)m_texture.getSize().y });
}

void GUIImage::ChangeTexture(const sf::Texture& _text) 
{
	m_texture = _text;
	m_shape.setTexture(&m_texture);
	m_shape.setTextureRect({ 0,0, (int)m_texture.getSize().x, (int)m_texture.getSize().y });
}

void GUIImage::CalculateRectangleShape()
{
	GUI::CalculateRectangleShape();
	//m_texture = TextureSearchFunction(m_textureName);
}

GUIImage::GUIImage(GUI& _gui) : GUI(_gui)
{
	if (_gui.m_className == "GUIImage")
	{
		GUIImage* image = (GUIImage*)&_gui;
		m_textureName = image->m_textureName;
		m_texture = image->m_texture;
	}
	else
	{
		m_textureName = "";
		m_texture.create(1, 1);
	}

	m_shape = _gui.m_shape;
	m_bottomRight = _gui.m_bottomRight;
	m_topLeft = _gui.m_topLeft;
	m_className = "GUIImage";
	m_parent = _gui.m_parent;
	m_children = _gui.m_children;
	m_content = _gui.m_content;
}

GUIImage::GUIImage(int _parent) : GUI(_parent)
{
	m_shape.setTexture(&m_texture);

	m_shape.setFillColor(sf::Color::Red);

	m_className = "GUIImage";
	m_content = m_content | GUIContent::IMAGE;
}

GUIImage::GUIImage(std::string _imgName, int _parent) : GUI(_parent)
{
	m_textureName = _imgName;
	m_texture = *TextureSearchFunction(_imgName);
	m_shape.setTexture(&m_texture);

	m_shape.setFillColor(sf::Color::Red);

	m_className = "GUIImage";
	m_content = m_content | GUIContent::IMAGE;
}

GUIImage::GUIImage(sf::Vector2f _bl, sf::Vector2f _tr, std::string _imgName, int _parent) : GUI(_bl, _tr, _parent)
{
	m_textureName = _imgName;
	m_texture = *TextureSearchFunction(_imgName);
	m_shape.setTexture(&m_texture);

	m_shape.setFillColor(sf::Color::Red);

	m_className = "GUIImage";
	m_content = m_content | GUIContent::IMAGE;
}