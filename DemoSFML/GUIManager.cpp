#include "GUIManager.h"
#include "GUI.h"
#include <fstream>
#include <iostream>

#pragma region ButtonCommands
void CallForHelp(GUI* _gui)
{
	std::cout << _gui->GetTag() << " is calling for help" << std::endl;
}
#pragma endregion

#pragma region Private
GUIManager* GUIManager::Instance;
bool GUIManager::Exist(GUI* _node)
{
	if (m_nodes.size() == 0)
	{
		return false;
	}

	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
	{
		if ((*it) == _node)
		{
			return true;
		}
	}

	return false;
}
void GUIManager::Add(GUI* _node)
{
	if (!Exist(_node))
	{
		m_nodes.push_back(_node);
	}
}
void GUIManager::AddButtonCommand(std::string _name, ButtonFunction _func)
{
	if (m_buttonCommands->find(_name) != m_buttonCommands->end())
	{
		(*m_buttonCommands)[_name] = _func;
		return;
	}

	m_buttonCommands->insert(std::pair<std::string, ButtonFunction>(_name, _func));
}
#pragma endregion

#pragma region Public

	#pragma region Static
	GUIManager* GUIManager::GetInstance()
	{
		if (Instance == nullptr)
		{
			Instance = new GUIManager();
		}
		return Instance;
	}
	GUI* GUIManager::GetNodeStatic(int _id)
	{
		return Instance->GetNode(_id);
	}
	void GUIManager::ButtonPressed(std::string _tag, GUI* _sender)
	{
		GUIManager* localInstance = GetInstance();

		if (localInstance->m_buttonCommands->find(_tag) == localInstance->m_buttonCommands->end() || (*localInstance->m_buttonCommands)[_tag] == nullptr)
		{
			return;
		}

		(*localInstance->m_buttonCommands)[_tag](_sender);
	}
	#pragma endregion

	#pragma region Init
	GUIManager::GUIManager()
	{
		m_currentGUI = -1;
		m_buttonCommands = new std::unordered_map<std::string, ButtonFunction>();
		LoadButtonCommands();
	}
	void GUIManager::LoadButtonCommands()
	{
		AddButtonCommand("Help", CallForHelp);
	}
	#pragma endregion

	#pragma region Add
	void GUIManager::AddNode(GUI* _rootNode)
	{
		Add(_rootNode);
	}
	int GUIManager::AddRoot(GUI* _root)
	{
		m_roots.push_back(_root);
		m_currentGUI = (int)m_roots.size() - 1;
		return (int)m_roots.size() - 1;
	}
	#pragma endregion

	#pragma region Getters
	int GUIManager::GetID(GUI* _ui)
	{
		if (_ui == nullptr)
		{
			return -1;
		}
		return _ui->GetID();
	}
	int GUIManager::GetNewID()
	{
		return (int)m_nodes.size();
	}
	int GUIManager::GetNodeIDByTag(std::string _tag) const
	{
		for (int i = 0; i < m_nodes.size(); i++)
		{
			if (m_nodes[i]->GetTag() == _tag)
			{
				return m_nodes[i]->GetID();
			}
		}

		return -1;
	}
	std::vector<int> GUIManager::GetRootIDs()
	{
		std::vector<int> ids;

		for (GUI* root : m_roots)
		{
			ids.push_back(GetID(root));
		}

		return ids;
	}
	GUI* GUIManager::GetNode(int _id)
	{
		for (int i = 0; i < m_nodes.size(); i++)
		{
			if (m_nodes[i]->GetID() == _id)
			{
				return m_nodes[i];
			}
		}

		return nullptr;
	}
	GUI* GUIManager::GetCurrentRoot()
	{
		if (m_roots.size() == 0 || m_currentGUI == -1)
		{
			return nullptr;
		}

		return GetNode(m_currentGUI);
	}
	GUI* GUIManager::GetNodeByTag(std::string _tag) const
	{
		for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
		{
			if ((*it)->GetTag() == _tag)
			{
				return (*it);
			}
		}

		return nullptr;
	}
	#pragma endregion

	void GUIManager::RemoveNode(GUI* _gui)
	{
		for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
		{
			if ((*it) == _gui)
			{
				m_nodes.erase(it);
				return;
			}
		}
	}

	void GUIManager::RemoveRoot(int _index)
	{
		for (int i = 0; i < m_roots.size(); i++)
		{
			if (GetID(m_roots[i]) == _index)
			{
				m_roots.erase(m_roots.begin() + i);
				i = (int)m_roots.size();
			}
		}
	}

	#pragma region Setters
	void GUIManager::SetCurrentRoot(int _curr)
	{
		m_currentGUI = _curr;
	}
	void GUIManager::SetCurrentRoot(GUI* _gui)
	{
		for (int i = 0; i < m_roots.size(); i++)
		{
			if (_gui == m_roots[i])
			{
				m_currentGUI = _gui->GetID();
				return;
			}
		}
	}
	void GUIManager::ResetIDs()
	{
		int id = 0;
		for (GUI* root : m_roots)
		{
			root->PrepareResetID();
		}

		for (GUI* root : m_roots)
		{
			root->ResetID(id);
		}
	}
	#pragma endregion

	#pragma region Misc
	bool GUIManager::NodeIsRoot(GUI* _node, int* _index)
	{
		for (auto root : m_roots)
		{
			if (_node == root)
			{
				return true;
			}
		}

		return false;
	}
	void GUIManager::ChangeNode(int index, GUI* _gui)
	{
		GUI* gui = GetNode(index);
		for (int i = 0; i < m_roots.size(); i++)
		{
			if (m_roots[i] == gui)
			{
				m_roots[i] = _gui;
			}
		}

		for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
		{
			if ((*it) == gui)
			{
				(*it) = _gui;
			}
		}
		delete gui;
	}
	#pragma endregion

	#pragma region Save Management
	void GUIManager::Save(std::ofstream& _stream)
	{
		_stream << m_nodes.size() << std::endl;
		for (auto root : m_roots)
		{
			_stream << GetID(root) << " ";
		}
		_stream << "RootsEnd";
		_stream <<  std::endl;

		for (auto node : m_nodes)
		{
			if (node != nullptr)
			{
				node->Save(_stream, *this);
			}
			
		}
	}
	void GUIManager::Load(std::ifstream& _stream)
	{
		m_nodes.clear();
		m_roots.clear();

		std::vector<int> rootIDs;

		std::string inputStr;
		int nodeCount;

		_stream >> inputStr;
		nodeCount = std::stoi(inputStr);
		m_nodes.reserve(nodeCount);

		while (inputStr != "RootsEnd")
		{
			_stream >> inputStr;
			if (inputStr != "RootsEnd")
			{
				rootIDs.push_back(std::stoi(inputStr));
			}
		}

		for (int i = 0; i < nodeCount; i++)
		{
			int id;

			_stream >> inputStr;
			_stream >> id;

			GUI* gui = nullptr;

			if (inputStr == "GUI")
			{
				gui = new GUI();
			}
			else if (inputStr == "GUIImage")
			{
				gui = new GUIImage();
			
			}
			else if (inputStr == "GUIText")
			{
				gui = new GUIText();
			}
			else if (inputStr == "GridLayout")
			{
				gui = new GUIGridLayout();
			}
			else if (inputStr == "GUIMaskingContainer")
			{
				gui = new GUIMaskingContainer();
			}

			gui->SetTag(gui->GetType() + std::to_string(id));
			gui->Load(_stream, *this);
			gui->SetID(id);
			m_nodes.push_back(gui);
		}

		for (int root : rootIDs)
		{
			m_roots.push_back(GetNode(root));
		}

		for (int i = 0; i < m_roots.size(); i++)
		{
			m_roots[i]->RecalculateFamilyShapes();
		}

		m_currentGUI = m_roots[0]->GetID();
	}

	void GUIManager::LoadPrefab(std::ifstream& _stream, int _baseParent)
	{
		if (_baseParent == -1)
		{
			Load(_stream);
			return;
		}

		int nodeNumber;
		std::vector<int> rootIDs;

		int baseId = -1;
		for (auto node : m_nodes)
		{
			if (node->GetID() >= baseId)
			{
				baseId = node->GetID() + 1;
			}
		}

		std::string inputStr;
		int inputInt;
		_stream >> inputStr;
		nodeNumber = std::stoi(inputStr);

		m_nodes.reserve(m_nodes.size() + nodeNumber);

		while (inputStr != "RootsEnd")
		{
			_stream >> inputStr;
			if (inputStr != "RootsEnd")
			{
				inputInt = std::stoi(inputStr);
				inputInt += baseId;
				rootIDs.push_back(inputInt);
			}
		}

		for (int i = 0; i < nodeNumber; i++)
		{
			int id;

			_stream >> inputStr;
			_stream >> id;

			id += baseId;

			GUI* gui = nullptr;

			if (inputStr == "GUI")
			{
				gui = new GUI();
			}
			else if (inputStr == "GUIImage")
			{
				gui = new GUIImage();

			}
			else if (inputStr == "GUIText")
			{
				gui = new GUIText();
			}
			else if (inputStr == "GridLayout")
			{
				gui = new GUIGridLayout();
			}
			else if (inputStr == "GUIMaskingContainer")
			{
				gui = new GUIMaskingContainer();
			}

			gui->SetTag(gui->GetType() + std::to_string(id));
			gui->Load(_stream, *this, baseId);
			gui->SetID(id);
			m_nodes.push_back(gui);
		}

		for (int root : rootIDs)
		{
			GetNode(root)->SetParent(_baseParent - 1);
			GetNode(_baseParent - 1)->AddChild(root);
		}

		for (int i = 0; i < m_roots.size(); i++)
		{
			m_roots[i]->RecalculateFamilyShapes();
		}
	}
	#pragma endregion

#pragma endregion