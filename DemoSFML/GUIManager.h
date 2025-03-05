#pragma once

#include <unordered_map>

#include "SFML/Graphics.hpp"

class GUI;

class GUIManager
{
public:
	typedef void (*ButtonFunction)(GUI*);

private:
	static GUIManager* Instance;

	std::vector<GUI*> m_roots;
	std::vector<GUI*> m_nodes;
	std::unordered_map<std::string, ButtonFunction>* m_buttonCommands;
	int m_currentGUI;
	bool Exist(GUI*);
	void Add(GUI*);

public:
	//Static
	static GUIManager* GetInstance();
	static GUI* GetNodeStatic(int);
	static void ButtonPressed(std::string, GUI*);

	//Init
	GUIManager();
	void LoadButtonCommands();

	//Add
	void AddNode(GUI*);
	int AddRoot(GUI*);
	void AddButtonCommand(std::string, ButtonFunction);

	//Getters
	int GetID(GUI*);
	int GetNewID();
	int GetNodeIDByTag(std::string) const;
	std::vector<int> GetRootIDs();
	GUI* GetNode(int);
	GUI* GetCurrentRoot();
	GUI* GetNodeByTag(std::string) const;
	std::vector<GUI*> GetRoots() const { return m_roots; };

	void RemoveNode(GUI*);
	void RemoveRoot(int);

	//Setters
	void SetCurrentRoot(int);
	void SetCurrentRoot(GUI*);
	void ResetIDs();

	//Misc
	bool NodeIsRoot(GUI*, int* = nullptr);
	void ChangeNode(int, GUI*);

	//Save Management
	void Save(std::ofstream&);
	void Load(std::ifstream&);
	void LoadPrefab(std::ifstream&, int);
};