#ifndef SCROLLMANAGER_H
#define SCROLLMANAGER_H

#include <string>
#include <unordered_map>
#include <sfml/Graphics.hpp>

class GUI;
class GUIMaskingContainer;
class GUIGridLayout;

class ScrollManager
{
public:
	struct ScrollStruct 
	{
		GUI* scrollInterface = nullptr;
		GUIMaskingContainer* mask = nullptr;
		GUIGridLayout* layout = nullptr;
		int baseRow = 0;
		float maxScroll = 0.0f;
	};

	ScrollManager() = default;
	~ScrollManager();

	static ScrollManager* GetInstance();

	void AddRow(std::string _name, std::string _toAdd);
	void RemoveRow(std::string _name);

	void CreateNewStruct(GUIMaskingContainer* _mask, GUIGridLayout* _layout, std::string _name);

	void Update();

	ScrollStruct* GetStruct(std::string _name);
private:
	std::unordered_map<std::string, ScrollStruct*> m_structs;
	static ScrollManager* m_instance;
};


#endif // !

