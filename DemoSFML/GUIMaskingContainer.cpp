#include "GUI.h"
#include "GUIManager.h"
#include "common.h"
#include "fstream"
#include <string>

GUIMaskingContainer::GUIMaskingContainer()
{
	m_renderTexture = new sf::RenderTexture();
	m_className = "GUIMaskingContainer";
	m_renderTexture->create(5000, 5000);
	m_view = new sf::View();
}

GUIMaskingContainer::GUIMaskingContainer(GUI& _gui) : GUI(_gui)
{
	if (_gui.GetType() == "GUIMaskingContainer")
	{
		GUIMaskingContainer& _container = (GUIMaskingContainer&)_gui;
		m_renderTexture = _container.m_renderTexture;
	}
	else
	{
		m_renderTexture = new sf::RenderTexture();
	}

	m_className = "GUIMaskingContainer";
	m_renderTexture->create(5000, 5000);
	m_view = new sf::View();
}

void GUIMaskingContainer::CalculateRectangleShape()
{
	sf::Vector2f pos;
	sf::Vector2f size;

	sf::Vector2f parentPos = (m_parent != -1) ? GUIManager::GetNodeStatic(m_parent)->GetShapePosition() : sf::Vector2f{ 0, 0 };
	sf::Vector2f parentSize = (m_parent != -1) ? GUIManager::GetNodeStatic(m_parent)->GetPixelSize() : sf::Vector2f{ (float)GameData::Window->getSize().x, (float)GameData::Window->getSize().y };
	pos.x = parentPos.x + parentSize.x * m_topLeft.x;
	pos.y = parentPos.y + parentSize.y * m_topLeft.y;

	size.x = parentSize.x * (m_bottomRight.x - m_topLeft.x);
	size.y = parentSize.y * (m_bottomRight.y - m_topLeft.y);

	m_shape.setPosition(pos);
	m_shape.setSize(size);

	if (m_children.size() == 0)
	{
		return;
	}

	GUI* gui = GUIManager::GetNodeStatic(m_children[0]);

	sf::Vector2f totalTopLeft = gui->GetTopLeft();
	sf::Vector2f totalBottomRight = gui->GetBottomRight();

	for (int i = 1; i < m_children.size(); i++)
	{
		gui = GUIManager::GetNodeStatic(m_children[i]);
		sf::Vector2f topLeft = gui->GetTopLeft();
		sf::Vector2f bottomRight = gui->GetBottomRight();
		if (topLeft.x < totalTopLeft.x)
		{
			totalTopLeft.x = topLeft.x;
		}
		if (topLeft.y < totalTopLeft.y)
		{
			totalTopLeft.y = topLeft.y;
		}
		if (bottomRight.x > totalBottomRight.x)
		{
			totalBottomRight.x = bottomRight.x;
		}
		if (bottomRight.y > totalBottomRight.y)
		{
			totalBottomRight.y = bottomRight.y;
		}
	}

	if (m_renderTexture == nullptr)
	{
		m_renderTexture = new sf::RenderTexture();
	}

	sf::Vector2f totalSize = totalBottomRight - totalTopLeft;
	totalSize.x *= parentSize.x;
	totalSize.y *= parentSize.y;
	
	m_view->setSize(GetPixelSize());

	m_renderTexture->clear(sf::Color::White);
}

void GUIMaskingContainer::SaveBaseData(std::ofstream& _stream, GUIManager& _manager)
{
	GUI::SaveBaseData(_stream, _manager);
	_stream << "Offset " << m_offset.x << " " << m_offset.y << std::endl;
}

void GUIMaskingContainer::LoadCommand(std::string _command, std::ifstream& _stream, GUIManager& _manager, int _baseParent)
{
	std::string inputStr;
	inputStr = _command;
	if (inputStr == "Offset")
	{
		_stream >> inputStr;
		m_offset.x = std::stof(inputStr);
		_stream >> inputStr;
		m_offset.y = std::stof(inputStr);
		return;
	}
	GUI::LoadCommand(_command, _stream, _manager, _baseParent);
}

void GUIMaskingContainer::GetTotalOffset(sf::Vector2f& _offset)
{
	if (m_parent == -1)
	{
		return;
	}

	GUI* parent = GUIManager::GetInstance()->GetNode(m_parent);
	parent->GetTotalOffset(_offset);
	_offset -= m_offset;
	_offset += m_shape.getPosition();
}

void GUIMaskingContainer::Display(sf::RenderTarget& _target)
{
	sf::Vector2f pixelSize = GetPixelSize();
	sf::Vector2f basePos = GetCornerPosition(TOP_LEFT);
	m_renderTexture->clear(sf::Color::Transparent);
	m_view->setCenter((GetPixelSize() / 2.f) + m_offset);
	m_renderTexture->setView(*m_view);

	for (int i = 0; i < m_children.size(); i++)
	{
		GUI* gui = GUIManager::GetNodeStatic(m_children[i]);
		gui->Display(*m_renderTexture);
	}

	m_renderTexture->display();
	m_shape.setTexture(&m_renderTexture->getTexture());
	//m_shape.setTextureRect({ (int)(GetCornerPosition(TOP_LEFT).x + m_offset.x), (int)(GetCornerPosition(TOP_LEFT).y + m_offset.y), (int)pixelSize.x, (int)pixelSize.y});
	_target.draw(m_shape);
}