#include "GUI.h"
#include "GUIManager.h"
#include <string>
#include <fstream>

#include "common.h"



#pragma region Constructors
GUI::GUI(GUI& _gui)
{
	m_shape = _gui.m_shape;
	m_bottomRight = _gui.m_bottomRight;
	m_topLeft = _gui.m_topLeft;
	m_className = "GUI";
	m_parent = _gui.m_parent;
	m_children = _gui.m_children;
	m_tag = _gui.m_tag;
	m_content = _gui.m_content;
	m_color = _gui.m_color;
	m_hoverColor = _gui.m_hoverColor;
	m_hoverEnabled = _gui.m_hoverEnabled;
	m_id = _gui.m_id;
}
GUI::GUI(int _parent) : m_parent(_parent)
{
	m_topLeft = { 0,0 };
	m_bottomRight = { 1,1 };

	m_color = sf::Color::White;
	m_hoverColor = sf::Color::White;
	m_hoverEnabled = false;

	m_shape.setFillColor(m_color);

	m_className = "GUI";
	m_tag = "New Gui";
	m_id = GUIManager::GetInstance()->GetNewID();
}
GUI::GUI(sf::Vector2f _tl, sf::Vector2f _br, int _parent) : m_parent(_parent)
{
	m_topLeft = _tl;
	m_bottomRight = _br;

	m_color = sf::Color::White;
	m_hoverColor = sf::Color::White;
	m_hoverEnabled = false;

	m_shape.setFillColor(m_color);

	m_className = "GUI";
	m_tag = "New Gui";
	m_id = GUIManager::GetInstance()->GetNewID();
}
#pragma endregion

#pragma region Shapes
void GUI::CalculateRectangleShape()
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
	
}
void GUI::RecalculateFamilyShapes()
{
	int id = GUIManager::GetInstance()->GetID(this);
	CalculateRectangleShape();
	for (int child : m_children)
	{
		GUI* node = GUIManager::GetInstance()->GetNode(child);

		node->RecalculateFamilyShapes();
	}
}
#pragma endregion

#pragma region Getters
#pragma region Positions
sf::Vector2f GUI::GetPixelSize()
{
	return m_shape.getSize();
}
sf::Vector2f GUI::GetShapePosition()
{
	return m_shape.getPosition();
}
sf::Vector2f GUI::GetCornerPosition(CORNER _corner)
{
	sf::Vector2f cornerPos;

	sf::Vector2f pos = m_shape.getPosition();
	sf::Vector2f size = GetPixelSize();

	cornerPos.x = (_corner == 0 || _corner == 3) ? pos.x : pos.x + size.x;
	cornerPos.y = (_corner <= 1) ? pos.y : pos.y + size.y;

	return cornerPos;
}
void GUI::GetTotalOffset(sf::Vector2f& _offset)
{
	if (m_parent == -1)
	{
		return;
	}

	GUI* parent = GUIManager::GetInstance()->GetNode(m_parent);
	parent->GetTotalOffset(_offset);
}
#pragma endregion

#pragma region Colors
sf::Color GUI::GetColor()
{
	return m_shape.getFillColor();
}
sf::Color GUI::GetHoverColor() const
{
	return m_hoverColor;
}
bool GUI::GetHoverEnabled() const
{
	return m_hoverEnabled;
}
#pragma endregion

#pragma region Misc
std::string GUI::GetTag(void) const
{
	return m_tag;
}
std::vector<int> GUI::GetChildren()
{
	return m_children;
}
int GUI::GetParent() const
{
	return m_parent;
}
std::string GUI::GetType()
{
	return m_className;
}
float GUI::GetNormalizedValue(GUI_SIDE _side)
{
	switch (_side)
	{
	case GUI_LEFT:
		return m_topLeft.x;
	case GUI_RIGHT:
		return m_bottomRight.x;
	case GUI_TOP:
		return m_topLeft.y;
	case GUI_BOTTOM:
		return m_bottomRight.y;
	default:
		return 0;
	}
}
bool GUI::HasContentType(GUIContent _content)
{
	return m_content & _content;
}

void GUI::GetFamilyList(std::vector<int>& _list)
{
	for (auto child : m_children)
	{
		_list.push_back(child);
		GUIManager::GetNodeStatic(child)->GetFamilyList(_list);
	}
}

void GUI::PrepareResetID()
{
	if (m_childrenBuffer != nullptr)
	{
		delete m_childrenBuffer;
	}

	m_childrenBuffer = new std::vector<GUI*>;

	for (int i = 0; i < m_children.size(); i++)
	{
		m_childrenBuffer->push_back(GUIManager::GetNodeStatic(m_children[i]));
		m_childrenBuffer->back()->PrepareResetID();
	}
}

int GUI::ResetID(int& _id)
{
	m_children.clear();

	for (int i = 0; i < m_childrenBuffer->size(); i++)
	{
		m_children.push_back(m_childrenBuffer->at(i)->ResetID(_id));
	}
	
	m_id = _id;
	_id++;

	for (int i = 0; i < m_childrenBuffer->size(); i++)
	{
		m_childrenBuffer->at(i)->SetParent(m_id);
	}

	delete m_childrenBuffer;

	return m_id;
}

#pragma endregion
#pragma endregion

#pragma region Setters

#pragma region Positions
void GUI::SetCornerPosition(sf::Vector2f _pos, int _corner)
{
	switch (_corner)
	{
	case TOP_LEFT:
		SetSidePosition(_pos, GUI_TOP);
		SetSidePosition(_pos, GUI_LEFT);
		break;
	case TOP_RIGHT:
		SetSidePosition(_pos, GUI_TOP);
		SetSidePosition(_pos, GUI_RIGHT);
		break;
	case BOTTOM_RIGHT:
		SetSidePosition(_pos, GUI_BOTTOM);
		SetSidePosition(_pos, GUI_RIGHT);
		break;
	case BOTTOM_LEFT:
		SetSidePosition(_pos, GUI_BOTTOM);
		SetSidePosition(_pos, GUI_LEFT);
		break;
	}
}
void GUI::SetSidePosition(sf::Vector2f _pos, int _side)
{
	switch (_side)
	{
	case GUI_LEFT:
		if (m_parent == -1)
		{
			m_topLeft.x = _pos.x / GameData::Window->getSize().x;
		}
		else
		{
			GUI* parent = GUIManager::GetInstance()->GetNode(m_parent);
			m_topLeft.x = (_pos.x - parent->GetShapePosition().x) / parent->GetPixelSize().x;
		}

		break;
	case GUI_RIGHT:
		if (m_parent == -1)
		{
			m_bottomRight.x = _pos.x / GameData::Window->getSize().x;
		}
		else
		{
			GUI* parent = GUIManager::GetInstance()->GetNode(m_parent);
			m_bottomRight.x = (_pos.x - parent->GetShapePosition().x) / parent->GetPixelSize().x;
		}
		break;

	case GUI_TOP:
		if (m_parent == -1)
		{
			m_topLeft.y = _pos.y / GameData::Window->getSize().y;
		}
		else
		{
			GUI* parent = GUIManager::GetInstance()->GetNode(m_parent);
			m_topLeft.y = (_pos.y - parent->GetShapePosition().y) / parent->GetPixelSize().y;
		}
		break;

	case GUI_BOTTOM:
		if (m_parent == -1)
		{
			m_bottomRight.y = _pos.y / GameData::Window->getSize().y;
		}
		else
		{
			GUI* parent = GUIManager::GetInstance()->GetNode(m_parent);
			m_bottomRight.y = (_pos.y - parent->GetShapePosition().y) / parent->GetPixelSize().y;
		}
		break;

	}
	RecalculateFamilyShapes();
}
#pragma endregion

#pragma region Colors
void GUI::SetColor(sf::Color _col)
{
	m_color = _col;
	m_shape.setFillColor(_col);
}

void GUI::SetColorAlpha(float _a)
{
	m_color.a = (sf::Uint8)_a;
	m_shape.setFillColor(m_color);
}
void GUI::SetHoverColor(sf::Color _col)
{
	m_hoverColor = _col;
}
void GUI::SetHoverColorAlpha(float _a)
{
	m_hoverColor.a = (sf::Uint8)_a;
}
void GUI::SetHoverEnabled(bool _enabled)
{
	m_hoverEnabled = _enabled;
}
#pragma endregion

#pragma region Misc
void GUI::SetTag(std::string _tag)
{
	if (_tag.size() > 128)
	{
		_tag = _tag.substr(0, 128);
	}

	m_tag = _tag;
}
void GUI::SetNormalizedValue(GUI_SIDE _side, float _value)
{
	switch (_side)
	{
	case GUI_LEFT:
		m_topLeft.x = _value;
		break;
	case GUI_RIGHT:
		m_bottomRight.x = _value;
		break;
	case GUI_TOP:
		m_topLeft.y = _value;
		break;
	case GUI_BOTTOM:
		m_bottomRight.y = _value;
		break;
	default:
		break;
	}

	RecalculateFamilyShapes();
}
void GUI::RemoveChild(int _child)
{
	for (int i = 0; i < m_children.size(); i++)
	{
		if (m_children.at(i) == _child)
		{
			m_children.erase(m_children.begin() + i);
			i = (int)m_children.size();
		}
	}
}

void GUI::SetID(int _id)
{
	m_id = _id;
}
#pragma endregion

#pragma endregion

void GUI::AddChild(int _child)
{
	m_children.push_back(_child);
}

#pragma region GameLoop
void GUI::Update(float _dt, sf::RenderWindow& _window, bool _hoverDetected, GUI* _hoverTarget)
{
	if (m_hoverEnabled && _hoverDetected && _hoverTarget == this)
	{
		m_shape.setFillColor(m_hoverColor);
	}
	else
	{
		m_shape.setFillColor(m_color);
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
void GUI::Update(float _dt, sf::RenderWindow& _window)
{
	GUI* target = nullptr;
	sf::Vector2i mousePos = sf::Mouse::getPosition(_window);
	bool clickDetected = DetectClick(mousePos, target);

	if (clickDetected && target == this)
	{
		if (m_hoverEnabled)
		{
			m_shape.setFillColor(m_hoverColor);
		}
		else
		{
			m_shape.setFillColor(m_color);
		}
	}
	else
	{
		m_shape.setFillColor(m_color);
	}

	for (int child : m_children)
	{
		GUI* gui = GUIManager::GetNodeStatic(child);
		gui->Update(_dt, _window, clickDetected, target);
	}
}
void GUI::Display(sf::RenderTarget& _window)
{
	if (!m_enabled)
	{
		return;
	}

	_window.draw(m_shape);

	for (int child : m_children)
	{
		GUIManager::GetNodeStatic(child)->Display(_window);
	}
}
#pragma endregion

#pragma region Clicks
bool GUI::DetectClick(sf::Vector2i _mousePos, GUI*& _target)
{
	if (m_ignoreClicks || !m_enabled)
	{
		return false;
	}
	sf::Vector2f offset;
	if (m_parent != -1)
	{
		GUIManager::GetNodeStatic(m_parent)->GetTotalOffset(offset);
	}
	
	_mousePos -= {(int)offset.x, (int)offset.y};
	for (int child : m_children)
	{
		if (GUIManager::GetNodeStatic(child)->DetectClick(_mousePos, _target))
		{
			return true;
		}
	}

	sf::Vector2f pos;
	sf::Vector2f size;
	sf::Vector2f tl;
	sf::Vector2f br;

	pos = m_shape.getPosition();
	size = m_shape.getSize();

	tl = pos;
	br = pos + size;

	if (_mousePos.x < tl.x || _mousePos.x > br.x || _mousePos.y < tl.y || _mousePos.y > br.y)
	{
		return false;
	}

	_target = this;
	return true;
}
void GUI::Click()
{
	GUIManager::ButtonPressed(m_tag, this);
}
#pragma endregion

#pragma region SaveManagement
void GUI::SaveBaseData(std::ofstream& _stream, GUIManager& _data)
{
	_stream << m_className << " " << _data.GetID(this) << std::endl;

	_stream << "Parent " << m_parent << std::endl;

	_stream << "TAG " << m_tag << " END" << std::endl;

	if (m_children.size() != 0)
	{
		_stream << "Children ";
		for (int i = 0; i < m_children.size(); i++)
		{
			_stream << m_children[i] << " ";
		}
		_stream << "ChildrenEnd";
		_stream << std::endl;
	}

	_stream << "TL " << m_topLeft.x << " " << m_topLeft.y << std::endl;
	_stream << "BR " << m_bottomRight.x << " " << m_bottomRight.y << std::endl;

	_stream << "COLOR " << (int)m_color.r << " " << (int)m_color.g << " " << (int)m_color.b << " " << (int)m_color.a << std::endl;

	_stream << "HOVERCOLOR " << (int)m_hoverColor.r << " " << (int)m_hoverColor.g << " " << (int)m_hoverColor.b << " " << (int)m_hoverColor.a << std::endl;

	_stream << "HOVERENABLED " << m_hoverEnabled << std::endl;

	_stream << "ENABLED " << (int)m_enabled << std::endl;

	_stream << "IGNORECLICKS " << (int)m_ignoreClicks << std::endl;
}
void GUI::Save(std::ofstream& _stream, GUIManager& _data)
{
	SaveBaseData(_stream, _data);
	_stream << "End" << std::endl;
}

void GUI::Load(std::ifstream& _stream, GUIManager& _manager, int _baseID)
{
	std::string inputStr;
	while (inputStr != "End")
	{
		_stream >> inputStr;

		LoadCommand(inputStr, _stream, _manager, _baseID);
	}
}

void GUI::LoadCommand(std::string _command, std::ifstream& _stream, GUIManager& _manager, int _baseID)
{
	int inputInt;
	std::string inputStr;

	if (_command == "Parent")
	{
		_stream >> inputInt;
		if (_baseID == -1)
		{
			m_parent = inputInt;
		}
		else
		{
			m_parent = inputInt + _baseID;
		}
		return;
	}
	else if (_command == "Children")
	{
		while (inputStr != "ChildrenEnd")
		{
			_stream >> inputStr;

			if (inputStr != "ChildrenEnd")
			{
				inputInt = std::stoi(inputStr);

				if (_baseID != -1)
				{
					inputInt += _baseID;
				}
				m_children.push_back(inputInt);
			}
		}
		return;
	}
	else if (_command == "TL")
	{
		_stream >> m_topLeft.x;
		_stream >> m_topLeft.y;
		return;
	}
	else if (_command == "BR")
	{
		_stream >> m_bottomRight.x;
		_stream >> m_bottomRight.y;
		return;
	}
	else if (_command == "COLOR")
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
		m_color = color;
		m_shape.setFillColor(color);
		return;
	}
	else if (_command == "TAG")
	{
		m_tag = "";

		while (inputStr != "END")
		{
			_stream >> inputStr;
			if (inputStr != "END")
			{
				if (m_tag.size() > 0)
				{
					m_tag += " ";
				}
				m_tag += inputStr;
			}
		}
		return;
	}
	else if (_command == "HOVERCOLOR")
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
		m_hoverColor = color;

		return;
	}
	else if (_command == "HOVERENABLED")
	{
		_stream >> m_hoverEnabled;
		return;
	}
	else if (_command == "IGNORECLICKS")
	{
		_stream >> inputInt;
		m_ignoreClicks = (bool)inputInt;
	}
	else if (_command == "ENABLED") 
	{
		_stream >> inputInt;
		m_enabled = (bool)inputInt;
	}
}
#pragma endregion

#pragma region friends
std::string& operator<<(std::string& _str, GUI& _gui)
{
	_str.clear();
	_str += _gui.m_className + " " + std::to_string(GUIManager::GetInstance()->GetID(&_gui)) + "\n";

	_str += "Parent " + std::to_string(_gui.m_parent) + "\n";

	if (_gui.m_children.size() != 0)
	{
		_str += "Children ";
		for (int i = 0; i < _gui.m_children.size(); i++)
		{
			_str += std::to_string(_gui.m_children[i]) += " ";
		}
		_str += "ChildrenEnd\n";
	}

	_str += "TL " + std::to_string(_gui.m_topLeft.x) + " " + std::to_string(_gui.m_topLeft.y) + "\n";
	_str += "BR " + std::to_string(_gui.m_bottomRight.x) + " " + std::to_string(_gui.m_bottomRight.y) + "\n";

	return _str;
}
#pragma endregion

