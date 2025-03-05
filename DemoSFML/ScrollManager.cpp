#include "ScrollManager.h"
#include "GUI.h"
#include "InputManager.h"
#include "GUIManager.h"
#include "common.h"
#include <fstream>

#define WHEEL_SPEED 7.5f

ScrollManager* ScrollManager::m_instance = nullptr;

ScrollManager::~ScrollManager()
{
	for (auto it = m_structs.begin(); it != m_structs.end(); it++)
	{
		delete it->second;
	}
	m_structs.clear();
}

ScrollManager* ScrollManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new ScrollManager();
	}
	return m_instance;
}

void ScrollManager::AddRow(std::string _name, std::string _toAdd)
{
	ScrollStruct* temp = m_structs[_name];

	GUIManager& gui = *GUIManager::GetInstance(); //get gui

	std::ifstream stream(_toAdd);

	for (int i = 0; i < temp->layout->GetColumnCount(); i++)
	{
		if (stream.is_open())
		{
			gui.LoadPrefab(stream, temp->layout->GetID() + 1);
			stream.seekg(0);
		}
	}
	stream.close();
	GUI* child = gui.GetNode(temp->layout->GetChildren()[0]);
	temp->maxScroll = (child->GetPixelSize().y + temp->layout->GetVerticalSpacing()) * ((temp->layout->GetChildren().size() / temp->layout->GetColumnCount()) - temp->baseRow);
}

void ScrollManager::RemoveRow(std::string _name)
{
	ScrollStruct* temp = m_structs[_name];

	GUIManager& gui = *GUIManager::GetInstance(); //get gui

	for (int i = 0; i < temp->layout->GetColumnCount(); i++)
	{
		temp->layout->RemoveChild(temp->layout->GetChildren()[temp->layout->GetChildren().size() - 1]);
	}
	GUI* child = gui.GetNode(temp->layout->GetChildren()[0]);
	temp->maxScroll = child->GetPixelSize().y * (temp->layout->GetRowCount() - temp->baseRow);

	GUIMaskingContainer* parent = (GUIMaskingContainer*)gui.GetNode(temp->layout->GetParent());
	if (parent->GetOffset().y > temp->maxScroll) parent->SetOffsetY(temp->maxScroll);
}

void ScrollManager::CreateNewStruct(GUIMaskingContainer* _mask, GUIGridLayout* _layout, std::string _name)
{
	if (m_structs.find(_name) != m_structs.end()) 
	{
		delete m_structs[_name];
		m_structs[_name] = nullptr;
		m_structs.erase(_name); 
	}
	ScrollStruct* temp = new ScrollStruct();
	temp->mask = _mask;
	temp->layout = _layout;
	temp->baseRow = _layout->GetRowCount();
	temp->scrollInterface = temp->mask;
	temp->maxScroll = 0;
	m_structs.insert({ _name,temp });
}

void ScrollManager::Update()
{
		auto inputs = InputManager::GetInstance();

		if (inputs->IsSinglePress("Wheel")) {
			GUIManager& gui = *GUIManager::GetInstance();
			for (auto i = m_structs.begin(); i != m_structs.end(); i++) 
			{
				if (i->second->mask->DetectClick((sf::Vector2i)GameData::GetMousePos(), i->second->scrollInterface))
				{
					i->second->mask->SetOffsetY(i->second->mask->GetOffset().y - (inputs->GetMouseWheelDelta() * WHEEL_SPEED));

					if (i->second->mask->GetOffset().y < 0) i->second->mask->SetOffsetY(0);
					if (i->second->mask->GetOffset().y > i->second->maxScroll) i->second->mask->SetOffsetY(i->second->maxScroll);
				}
			}
		}
}

ScrollManager::ScrollStruct* ScrollManager::GetStruct(std::string _name)
{
	if (m_structs.find(_name) != m_structs.end())
	{
		return m_structs[_name];
	}
	else 
	{
		return nullptr;
	}
}
