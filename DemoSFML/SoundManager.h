#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <string>
#include <SFML/Audio.hpp>
#include <unordered_map>

enum SoundType 
{
	NO_SOUND = -1,
	MUSIC,
	INTERACTIONS,
	UI,
	AMBIENT
};

enum GameStateType 
{
	MENUS,
	GAME,
	DIALOGUE
};

struct AudioStruct
{
	std::string path;
	SoundType type = MUSIC;
	size_t id = 0;

	bool manualVolume = false;

	sf::SoundBuffer* buffer = nullptr;
	sf::Sound* sound = nullptr;

	sf::Music* music = nullptr;

	AudioStruct() = default;
	AudioStruct(AudioStruct* _copy) 
	{
		path = _copy->path;
		type = _copy->type;
		id = _copy->id;
		if(type == MUSIC || type == AMBIENT)
		{
			music = new sf::Music();
			music->openFromFile(path);
			music->setVolume(_copy->music->getVolume());
			music->setLoop(_copy->music->getLoop());
			sound = nullptr;
		}
		else
		{
			sound = new sf::Sound();
			sound->setBuffer(*_copy->buffer);
			sound->setVolume(_copy->sound->getVolume());
			sound->setLoop(_copy->sound->getLoop());
			music = nullptr;
		}
	}
};

class SoundManager
{
public:
	SoundManager() = default;
	~SoundManager();
	static SoundManager* GetInstance();
	static bool HasAudioOutput();

	void Play(std::string _name , bool _oneTime = false);
	void Pause(std::string _name);
	void Stop(std::string _name);
	void StopAll();

	std::vector<AudioStruct*> GetPlayingAudios() { return playingAudios; }

	void Init(std::string _path);
	void Update();

	void ChangeState(GameStateType _state);
	const GameStateType GetState() { return state; }

	void SetVolumeAccordingToValue(std::string _name, float _value, float _offset = 0.f);

	void SetVolume(float _volume) { m_volume = _volume; }
	const float GetVolume() { return m_volume; }
private:
	const float GetVolume(SoundType _type);
	static SoundManager* instance;
	GameStateType state = MENUS;
	std::unordered_map<size_t,AudioStruct*> audioList;
	static std::hash<std::string> hasher;
	std::vector<AudioStruct*> playingAudios;
	float m_volume = 100.f;
};

#endif // !SOUNDMANAGER_H