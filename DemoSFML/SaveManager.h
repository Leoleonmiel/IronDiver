#include <string>
class SaveManager
{
public:
    SaveManager(); 

    ~SaveManager() {}

    static void Save();
    static void Load();
    static void SaveCharacter(std::string);
    static bool CanLoad();

    static std::string LastSavePath;
    static bool loadAfterInit;
private:
};
