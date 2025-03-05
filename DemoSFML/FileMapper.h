#ifndef FILE_MAPPER_H
#define FILE_MAPPER_H

#include <iostream>
#include <unordered_map>

class FileMapper
{
public:
	FileMapper() {};
	FileMapper(std::string _filePath);
	~FileMapper() {}

	void Load(std::string _filePath);

	int GetData(std::string _actionName) { return m_dataList[_actionName]; }

private:

	std::unordered_map<std::string, int> m_dataList;
};

#endif // !INPUT_MANGER_H