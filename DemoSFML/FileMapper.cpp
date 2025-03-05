#include "FileMapper.h"
#include <fstream>
#include <string>

FileMapper::FileMapper(std::string _filePath)
{
	Load(_filePath);
}

void FileMapper::Load(std::string _filePath)
{
	std::ifstream fileData(_filePath);

	if (fileData.is_open())
	{
		std::string line;
		std::string actionName;
		std::string inputName;

		do
		{
			fileData >> actionName;
			fileData >> inputName;

			m_dataList[actionName] = std::stoi(inputName);

		} while (std::getline(fileData, line));

		fileData.close();
	}
	else
	{
		std::cerr << "Erreur lors de l'ouverture du fichier :\n" << _filePath << std::endl;
	}
}
