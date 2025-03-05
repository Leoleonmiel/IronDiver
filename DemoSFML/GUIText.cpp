#include "GUI.h"
#include "GUIManager.h"
#include <fstream>
#include <iostream>

sf::Font* GUIText::Font;

void GUIText::SaveBaseData(std::ofstream& _stream, GUIManager& _manager)
{
	GUIImage::SaveBaseData(_stream, _manager);

	_stream << "TEXT" << std::endl;
	_stream << m_script << std::endl;
	_stream << "END" << std::endl;

	_stream << "FONTSIZE " << m_fontSize << std::endl;
	_stream << "FONTCOLOR " << (int)m_fontColor.r << " " << (int)m_fontColor.g << " " << (int)m_fontColor.b << " " << (int)m_fontColor.a << std::endl;
	_stream << "FONTHOVERCOLOR " << (int)m_fontHoverColor.r << " " << (int)m_fontHoverColor.g << " " << (int)m_fontHoverColor.b << " " << (int)m_fontHoverColor.a << std::endl;
	_stream << "MARGIN " << m_margin.x << " " << m_margin.y << std::endl;
}

void GUIText::CalculateRectangleShape()
{
	GUIImage::CalculateRectangleShape();
	UpdateText();
}

void GUIText::Update(float _dt, sf::RenderWindow& _window, bool _hoverDetected, GUI* _hoverTarget)
{
	if (m_hoverEnabled && _hoverDetected && _hoverTarget == this)
	{
		m_shape.setFillColor(m_hoverColor);
		m_text.setFillColor(m_fontHoverColor);
	}
	else
	{
		m_shape.setFillColor(m_color);
		m_text.setFillColor(m_fontColor);
	}

	for (int child : m_children)
	{
		GUI* gui = GUIManager::GetNodeStatic(child);
		if (gui != nullptr)
		{
			gui->Update(_dt, _window, _hoverDetected, _hoverTarget);
		}
	}
}

void GUIText::SetInMiddle(sf::RectangleShape& _shape)
{
	const sf::FloatRect bounds(m_text.getLocalBounds());
	const sf::Vector2f box(_shape.getSize());
	m_text.setOrigin((bounds.width - box.x) / 2 + bounds.left, (bounds.height - box.y) / 2 + bounds.top);
}

GUIText::GUIText(GUI& _gui) : GUIImage(_gui)
{
	if (_gui.GetType() == "GUIText")
	{
		GUIText& text = (GUIText&)_gui;
		m_script = text.m_script;
		m_fontColor = text.m_fontColor;
		m_fontHoverColor = text.m_fontHoverColor;
		m_fontSize = text.m_fontSize;
	}
	else
	{
		m_script = "";
		m_fontSize = 60;
		m_fontColor = sf::Color::White;
	}

	m_className = "GUIText";
	m_content = m_content | GUIContent::TEXT;
	UpdateText();
}

GUIText::GUIText(int _parent) : GUIImage(_parent)
{

	m_script = "";
	m_fontSize = 60;
	m_fontColor = sf::Color::White;
	m_fontHoverColor = sf::Color::White;
	m_className = "GUIText";
	m_content = m_content | GUIContent::TEXT;
	UpdateText();
}

GUIText::GUIText(std::string _imgPath, int _parent) : GUIImage(_imgPath, _parent)
{
	m_script = "";
	m_fontSize = 60;
	m_fontColor = sf::Color::White;
	m_fontHoverColor = sf::Color::White;
	m_className = "GUIText";
	m_content = m_content | GUIContent::TEXT;
	UpdateText();
}

GUIText::GUIText(std::string _script, sf::Color _color, int _size, int _parent) : GUIImage(_parent)
{
	m_script = _script;
	m_fontColor = _color;
	m_fontHoverColor = m_fontColor;
	m_fontSize = _size;
	m_className = "GUIText";
	m_content = m_content | GUIContent::TEXT;
	UpdateText();
}

GUIText::GUIText(std::string _imgPath, std::string _script, sf::Color _color, int _size, int _parent) : GUIImage(_imgPath, _parent)
{
	m_script = _script;
	m_fontColor = _color;
	m_fontHoverColor = m_fontColor;
	m_fontSize = _size;
	m_className = "GUIText";
	m_content = m_content | GUIContent::TEXT;
	UpdateText();
}

void GUIText::SetFontSize(int _size)
{
	m_fontSize = _size;
	UpdateText();
}

int GUIText::GetFontSize() const
{
	return m_fontSize;
}

void GUIText::SetText(std::string _script)
{
	m_script = _script;
	UpdateText();
}

std::string GUIText::GetText() const
{
	return m_script;
}

void GUIText::SetFontColor(sf::Color _color)
{
	m_fontColor = _color;
	m_fontHoverColor = m_fontColor;
	UpdateText();
}

sf::Color GUIText::GetFontColor() const
{
	return m_fontColor;
}

sf::Color GUIText::GetFontHoverColor() const
{
	return m_fontHoverColor;
}

void GUIText::SetFontHoverColor(sf::Color _color)
{
	m_fontHoverColor = _color;
}

void GUIText::SetMargin(sf::Vector2f _margin)
{
	m_margin = _margin;
	UpdateText();
}

void GUIText::SetMarginX(float _x)
{
	m_margin.x = _x;
	UpdateText();
}

void GUIText::SetMarginY(float _y)
{
	m_margin.y = _y;
	UpdateText();
}

sf::Vector2f GUIText::GetMargin() const
{
	return m_margin;
}

bool GUIText::GetSizeUp() const
{
	return m_sizeUp;
}

void GUIText::SetSizeUp(bool _value)
{
	m_sizeUp = _value;
}

int GUIText::GetFontSizeUpSize() const
{
return m_fontSizeUpSize;
}

void GUIText::SetFontSizeUpSize(int _value)
{
	m_fontSizeUpSize = _value;
}

void GUIText::UpdateText()
{
	m_text.setString(m_script);
	m_text.setCharacterSize(m_fontSize);
	m_text.setFillColor(m_fontColor);
	m_text.setPosition(GetShapePosition() + m_margin);
	m_text.setFont(*Font);
}

void GUIText::LoadCommand(std::string _command, std::ifstream& _stream, GUIManager& _manager, int _baseParent)
{
	std::string inputStr;
	inputStr = _command;

	if (inputStr == "TEXT")
	{
		m_script = "";
		do
		{
			_stream >> inputStr;
			if (inputStr != "END")
			{
				m_script += inputStr + " ";
			}
		} while (inputStr != "END");
		return;
	}
	else if (inputStr == "FONTSIZE")
	{
		_stream >> m_fontSize;
		return;
	}
	else if (inputStr == "FONTCOLOR")
	{
		sf::Color color;
		_stream >> inputStr;
		color.r = std::stoi(inputStr);
		_stream >> inputStr;
		color.g = std::stoi(inputStr);
		_stream >> inputStr;
		color.b = std::stoi(inputStr);
		_stream >> inputStr;
		color.a = std::stoi(inputStr);
		m_fontColor = color;
		return;
	}
	else if (inputStr == "FONTHOVERCOLOR")
	{
		sf::Color color;
		_stream >> inputStr;
		color.r = std::stoi(inputStr);
		_stream >> inputStr;
		color.g = std::stoi(inputStr);
		_stream >> inputStr;
		color.b = std::stoi(inputStr);
		_stream >> inputStr;
		color.a = std::stoi(inputStr);
		m_fontHoverColor = color;
		return;
	}
	else if (inputStr == "MARGIN")
	{
		_stream >> m_margin.x;
		_stream >> m_margin.y;
		return;
	}

	GUIImage::LoadCommand(_command, _stream, _manager, _baseParent);
}

void GUIText::Update(float _dt, sf::RenderWindow& _window)
{
	GUI* target = nullptr;
	sf::Vector2i mousePos = sf::Mouse::getPosition(_window);
	bool clickDetected = DetectClick(mousePos, target);

	if (clickDetected && target == this)
	{
		m_shape.setFillColor(m_hoverColor);
		m_text.setFillColor(m_fontHoverColor);
		if (m_sizeUp) 
		{
			m_text.setCharacterSize(m_fontSizeUpSize);
		}
	}
	else
	{
		m_text.setCharacterSize(m_fontSize);
		m_shape.setFillColor(m_color);
		m_text.setFillColor(m_fontColor);
	}

	for (int child : m_children)
	{
		GUI* gui = GUIManager::GetNodeStatic(child);
		gui->Update(_dt, _window, clickDetected, target);
	}
}

void GUIText::Display(sf::RenderTarget& _window)
{
	if (!m_enabled)
	{
		return;
	}

	_window.draw(m_shape);
	_window.draw(m_text);

	for (int child : m_children)
	{
		GUI* childGUI = GUIManager::GetInstance()->GetNode(child);
		childGUI->Display(_window);
	}
}

