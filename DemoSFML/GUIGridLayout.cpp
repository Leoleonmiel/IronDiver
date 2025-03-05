#include "GUI.h"
#include <fstream>
#include "GUIManager.h"
#include "common.h"

void GUIGridLayout::CalculateRectangleShape()
{
	sf::Vector2f pos;
	sf::Vector2f size;

	sf::Vector2f usableSize;
	sf::Vector2f childSize;
	sf::Vector2f childPos;

	sf::Vector2f parentPos = (m_parent != -1) ? GUIManager::GetNodeStatic(m_parent)->GetShapePosition() : sf::Vector2f{ 0, 0 };
	sf::Vector2f parentSize = (m_parent != -1) ? GUIManager::GetNodeStatic(m_parent)->GetPixelSize() : sf::Vector2f{ (float)GameData::Window->getSize().x, (float)GameData::Window->getSize().y };
	pos.x = parentPos.x + parentSize.x * m_topLeft.x;
	pos.y = parentPos.y + parentSize.y * m_topLeft.y;

	size.x = parentSize.x * (m_bottomRight.x - m_topLeft.x);
	size.y = parentSize.y * (m_bottomRight.y - m_topLeft.y);

	m_shape.setPosition(pos);
	m_shape.setSize(size);
}

void GUIGridLayout::CalculateRectangleShapeWithFamily()
{
	sf::Vector2f pos;
	sf::Vector2f size;

	sf::Vector2f usableSize;
	sf::Vector2f childSize;
	sf::Vector2f childPos;

	sf::Vector2f parentPos = (m_parent != -1) ? GUIManager::GetNodeStatic(m_parent)->GetShapePosition() : sf::Vector2f{ 0, 0 };
	sf::Vector2f parentSize = (m_parent != -1) ? GUIManager::GetNodeStatic(m_parent)->GetPixelSize() : sf::Vector2f{ (float)GameData::Window->getSize().x, (float)GameData::Window->getSize().y };
	pos.x = parentPos.x + parentSize.x * m_topLeft.x;
	pos.y = parentPos.y + parentSize.y * m_topLeft.y;

	size.x = parentSize.x * (m_bottomRight.x - m_topLeft.x);
	size.y = parentSize.y * (m_bottomRight.y - m_topLeft.y);

	m_shape.setPosition(pos);
	m_shape.setSize(size);

	usableSize.x = size.x - m_padding[GUI_LEFT] - m_padding[GUI_RIGHT] - m_horizontalSpacing * (m_columnCount - 1);
	usableSize.y = size.y - m_padding[GUI_TOP] - m_padding[GUI_BOTTOM] - m_verticalSpacing * (m_rowCount - 1);

	childSize.x = usableSize.x / m_columnCount;
	childSize.y = usableSize.y / m_rowCount;

	for (int i = 0; i < m_children.size(); i++)
	{
		int x = i % m_columnCount;
		int y = (int)std::floor(i / (float)m_columnCount);

		GUI* gui = GUIManager::GetNodeStatic(m_children[i]);

		childPos.x = m_padding[GUI_LEFT] + (x * (childSize.x + m_horizontalSpacing));
		childPos.y = m_padding[GUI_TOP] + (y * (childSize.y + m_verticalSpacing));
		childPos += pos;

		gui->SetCornerPosition(childPos, TOP_LEFT);

		childPos += childSize;

		gui->SetCornerPosition(childPos, BOTTOM_RIGHT);
	}
}

void GUIGridLayout::SaveBaseData(std::ofstream& _stream, GUIManager& _manager)
{
	GUI::SaveBaseData(_stream, _manager);

	_stream << "Padding " << m_padding[0] << " " << m_padding[1] << " " << m_padding[2] << " " << m_padding[3] << std::endl;
	_stream << "Spacing " << m_horizontalSpacing << " " << m_verticalSpacing << std::endl;
	_stream << "ColumnCount " << m_columnCount << std::endl;
	_stream << "RowCount " << m_rowCount << std::endl;
}

GUIGridLayout::GUIGridLayout(GUI& _gui) : GUI(_gui)
{
	if (_gui.m_className == "GridLayout")
	{
		GUIGridLayout* grid = (GUIGridLayout*)&_gui;
		memcpy(m_padding, grid->m_padding, 4 * sizeof(int));
		m_horizontalSpacing = grid->m_horizontalSpacing;
		m_verticalSpacing = grid->m_verticalSpacing;
		m_columnCount = grid->m_columnCount;
		m_rowCount = grid->m_rowCount;
		m_className = "GridLayout";
		return;
	}

	m_padding[0] = 0;
	m_padding[1] = 0;
	m_padding[2] = 0;
	m_padding[3] = 0;
	m_horizontalSpacing = 0;
	m_verticalSpacing = 0;
	m_columnCount = 5;
	m_rowCount = 5;
	m_className = "GridLayout";
}

GUIGridLayout::GUIGridLayout(int _parent) : GUI(_parent)
{
	m_className = "GridLayout";
	m_padding[0] = 0;
	m_padding[1] = 0;
	m_padding[2] = 0;
	m_padding[3] = 0;
	m_horizontalSpacing = 0;
	m_verticalSpacing = 0;
	m_columnCount = 5;
	m_rowCount = 5;
}

GUIGridLayout::GUIGridLayout(sf::Vector2f _tl, sf::Vector2f _br, int _parent) : GUI(_tl, _br, _parent)
{
	m_className = "GridLayout";
	m_padding[0] = 0;
	m_padding[1] = 0;
	m_padding[2] = 0;
	m_padding[3] = 0;
	m_horizontalSpacing = 0;
	m_verticalSpacing = 0;

	m_columnCount = 5;
	m_rowCount = 5;
}

void GUIGridLayout::RecalculateFamilyShapes()
{
	CalculateRectangleShapeWithFamily();
	for (int child : m_children)
	{
		GUI* node = GUIManager::GetInstance()->GetNode(child);

		node->RecalculateFamilyShapes();
	}
}

void GUIGridLayout::LoadCommand(std::string _command, std::ifstream& _stream, GUIManager& _manager, int _baseParent)
{
	std::string inputStr;
	inputStr = _command;
	if (inputStr == "Padding")
	{
		_stream >> m_padding[0];
		_stream >> m_padding[1];
		_stream >> m_padding[2];
		_stream >> m_padding[3];
		return;
	}
	else if (inputStr == "Spacing")
	{
		_stream >> m_horizontalSpacing >> m_verticalSpacing;
		return;
	}
	else if (inputStr == "ColumnCount")
	{
		_stream >> m_columnCount;
		return;
	}
	else if (inputStr == "RowCount")
	{
		_stream >> m_rowCount;
		return;
	}

	GUI::LoadCommand(_command, _stream, _manager, _baseParent);
}

void GUIGridLayout::Display(sf::RenderTarget& _window)
{
	if (!m_enabled)
	{
		return;
	}

	for (int child : m_children)
	{
		GUI* childGUI = GUIManager::GetInstance()->GetNode(child);
		childGUI->Display(_window);
	}
}

int GUIGridLayout::GetPadding(GUI_SIDE _side) const
{
	return m_padding[_side];
}

int GUIGridLayout::GetHorizontalSpacing() const
{
	return m_horizontalSpacing;
}

int GUIGridLayout::GetVerticalSpacing() const
{
	return m_verticalSpacing;
}

int GUIGridLayout::GetColumnCount() const
{
	return m_columnCount;
}

int GUIGridLayout::GetRowCount() const
{
	return m_rowCount;
}

void GUIGridLayout::SetPadding(GUI_SIDE _side, int _value)
{
	m_padding[_side] = _value;
	CalculateRectangleShapeWithFamily();
}

void GUIGridLayout::SetHorizontalSpacing(int _spacing)
{
	m_horizontalSpacing = _spacing;
	CalculateRectangleShapeWithFamily();
}

void GUIGridLayout::SetVerticalSpacing(int _spacing)
{
	m_verticalSpacing = _spacing;
	CalculateRectangleShapeWithFamily();
}

void GUIGridLayout::SetColumnCount(int _count)
{
	m_columnCount = _count;
	CalculateRectangleShapeWithFamily();
}

void GUIGridLayout::SetRowCount(int _count)
{
	m_rowCount = _count;
	CalculateRectangleShapeWithFamily();
}
