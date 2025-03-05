#include "DialogModule.h"
#include "GUI.h"
#include "GUIManager.h"
#include "common.h"

#include <iostream>
#include <stdio.h>

float DialogMaster::m_clock;
Dialog* DialogMaster::m_currentDialog = nullptr;
GUI* DialogMaster::m_dialogPannel = nullptr;
bool DialogMaster::m_dialogActive = false;
bool DialogMaster::m_dialogRunning = false;

void DialogMaster::InitialiseDialogMaster()
{
	m_clock = 0.f;
	m_currentDialog = nullptr;
	m_dialogPannel = GUIManager::GetInstance()->GetNodeByTag("BigDialog");
	if (m_dialogPannel == nullptr)
	{
		printf("dialog pannel not found\n");
	}
}

void DialogMaster::SetEnabled(bool _enabled)
{
	m_dialogPannel->SetEnabled(_enabled);
}

bool DialogMaster::TrySetDialog(Dialog& _dialog)
{

	if (m_currentDialog != nullptr)
	{
		delete m_currentDialog;
	}

	m_currentDialog = new Dialog(_dialog);
	if (m_currentDialog->instant)
	{
		GUIText* textGUI = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("DialogText");
		if (textGUI != nullptr)
		{
			textGUI->SetText(m_currentDialog->text);
		}
	}
	else
	{
		m_clock = 0.f;
	}

	GUIText* nameGUI = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("DialogName");
	if (nameGUI != nullptr)
	{
		nameGUI->SetText(m_currentDialog->name);
	}

	m_dialogActive = true;
	m_dialogRunning = true;

	return false;
}

std::string TextEmplaceer(sf::Text* _text, sf::RectangleShape* _shape, std::string _originalText)
{
	//If the text globalBounds is bigger than the shape, make a \n in the text at the right position
	if (_text->getGlobalBounds().width > _shape->getGlobalBounds().width)
	{
		std::string realText = _originalText;
		sf::Text tempText(*_text);
		tempText.setString("");
		for (int i = 0; i < _originalText.size(); i++)
		{
			//Add characters one by one and check if the text is bigger than the shape
			tempText.setString(tempText.getString() + realText[i]);
			if (tempText.getGlobalBounds().width > _shape->getGlobalBounds().width)
			{
				// Search at the last space to not cut a word
				while (realText[i] != ' ')
				{
					i--;
				}
				realText.erase(i, 1);
				realText.insert(i, "\n");
				//cut text at i to recall TextEmplacer with the following text
				realText = realText.substr(i);
				_originalText = _originalText.substr(0, i);
				return std::string(_originalText + TextEmplaceer(_text, _shape, realText));
			}
		}
	}
	return _originalText;
}

void DialogMaster::UpdateDialog()
{
	GUIText* textGUI = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("DialogText");
	float percent;
	int letterCount;

	m_clock += GameData::DeltaTime * GameData::GameSpeed;
	if (m_currentDialog == nullptr || m_currentDialog->instant || textGUI == nullptr || !m_dialogActive)
	{
		return;
	}

	textGUI->SetEnabled(true);
	percent = m_clock / (m_currentDialog->text.size() / m_currentDialog->speed);
	if (percent > 1.f || !m_dialogRunning)
	{
		percent = 1.f;
		m_dialogRunning = false;
	}
	letterCount = (int)(m_currentDialog->text.size() * percent);
	
	textGUI->SetText(m_currentDialog->text.substr(0, letterCount));
	textGUI->SetText(TextEmplaceer(textGUI->GetSfText(), textGUI->GetSfShape(), textGUI->GetText()));
}

void DialogMaster::PassDialog()
{
	GUIText* textGUI = (GUIText*)GUIManager::GetInstance()->GetNodeByTag("DialogText");
	if (textGUI == nullptr || m_currentDialog == nullptr || m_currentDialog->instant)
	{
		m_dialogActive = false;
		return;
	}

	if (m_dialogRunning)
	{
		m_dialogRunning = false;
	}
	else
	{
		m_dialogActive = false;
		SetEnabled(false);
	}
}

bool DialogMaster::DialogActive()
{
	return m_dialogActive;
}
