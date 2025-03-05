#include "SoundManager.h"
#include <fstream>
#include <iostream>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>

#define MAX_DB 0.0f
#define MIN_DB -16.0f

SoundManager* SoundManager::instance = nullptr;
std::hash<std::string> SoundManager::hasher;

SoundManager* SoundManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new SoundManager();
	}
	return instance;
}

bool SoundManager::HasAudioOutput()
{
		HRESULT hr;
		IMMDeviceEnumerator* pEnumerator = NULL;
		IMMDeviceCollection* pCollection = NULL;

		// Initialize COM
		hr = CoInitialize(NULL);
		if (FAILED(hr)) {
			//std::cerr << "CoInitialize failed: " << hr << std::endl;
			return false;
		}

		// Create a multimedia device enumerator
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
			CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
			(void**)&pEnumerator);
		if (FAILED(hr)) {
			//std::cerr << "CoCreateInstance(IMMDeviceEnumerator) failed: " << hr << std::endl;
			CoUninitialize();
			return false;
		}

		// Enumerate the audio output devices
		hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
		if (FAILED(hr)) {
			//std::cerr << "EnumAudioEndpoints failed: " << hr << std::endl;
			pEnumerator->Release();
			CoUninitialize();
			return false;
		}

		UINT count;
		hr = pCollection->GetCount(&count);
		if (FAILED(hr)) {
			//std::cerr << "GetCount failed: " << hr << std::endl;
			pCollection->Release();
			pEnumerator->Release();
			CoUninitialize();
			return false;
		}

		if (count == 0) {
			//std::cout << "No audio output devices found." << std::endl;
			pCollection->Release();
			pEnumerator->Release();
			CoUninitialize();
			return false;
		}
		else {
			//std::cout << "Audio output devices:" << std::endl;
			for (UINT i = 0; i < count; i++) {
				IMMDevice* pDevice;
				hr = pCollection->Item(i, &pDevice);
				if (FAILED(hr)) {
					//std::cerr << "Item failed: " << hr << std::endl;
					continue;
				}

				LPWSTR pwszID;
				hr = pDevice->GetId(&pwszID);
				if (FAILED(hr)) {
					//std::cerr << "GetId failed: " << hr << std::endl;
					pDevice->Release();
					continue;
				}

				// Display device ID
				//std::wcout << "Device " << i << ": " << pwszID << std::endl;

				CoTaskMemFree(pwszID);
				pDevice->Release();
			}
		}

		pCollection->Release();
		pEnumerator->Release();
		CoUninitialize();
		return true;
}

void SoundManager::Play(std::string _name, bool _oneTime)
{
	if (!HasAudioOutput()) return;
	size_t hash = hasher(_name);

	if (audioList.find(hash) == audioList.end())
	{
		//std::cout << "Audio not found: " << _name << std::endl;
		return;
	}
	if (_oneTime)
	{
		//Check if temp is already in the playing audios
		for (size_t i = 0; i < playingAudios.size(); i++)
		{
			if (playingAudios[i]->id == audioList[hash]->id) return;
		}
	}

	AudioStruct* temp = new AudioStruct(audioList[hash]);

	if (temp->type == MUSIC || temp->type == AMBIENT) temp->music->setVolume(GetVolume(temp->type));
	else temp->sound->setVolume(GetVolume(temp->type));

	playingAudios.push_back(temp);

	switch (playingAudios.back()->type)
	{
	case MUSIC:
	case AMBIENT:
		playingAudios.back()->music->play();
		break;
	case INTERACTIONS:
	case UI:
		playingAudios.back()->sound->play();
		break;
	default:
		break;
	}
}

void SoundManager::Pause(std::string _name)
{
	size_t hash = hasher(_name);
	for (int i = 0; i < playingAudios.size(); i++)
	{
		if (hash == playingAudios[i]->id)
		{
			if (playingAudios[i]->type == MUSIC || playingAudios[i]->type == AMBIENT) playingAudios[i]->music->pause();
			else playingAudios[i]->sound->pause();
			break;
		}
	}
}

void SoundManager::Stop(std::string _name)
{
	size_t hash = hasher(_name);
	for (int i = 0; i < playingAudios.size(); i++)
	{
		if (hash == playingAudios[i]->id) 
		{
			if (playingAudios[i]->type == MUSIC || playingAudios[i]->type == AMBIENT) playingAudios[i]->music->stop();
			else playingAudios[i]->sound->stop();
			break;
		}
	}
}

void SoundManager::StopAll()
{

	for (int i = 0; i < playingAudios.size(); i++)
	{
		if (playingAudios[i]->type == MUSIC || playingAudios[i]->type == AMBIENT) playingAudios[i]->music->stop();
		else playingAudios[i]->sound->stop();
	}
}

SoundType GetSoundType(std::string _type)
{
	if (_type == "MUSIC") return MUSIC;
	else if (_type == "INTERACTIONS") return INTERACTIONS;
	else if (_type == "UI") return UI;
	else if (_type == "AMBIENT") return AMBIENT;
	return NO_SOUND;
}

void SoundManager::Init(std::string _path)
{
	std::fstream file(_path, std::ios::in);
	if (file.is_open())
	{
		std::string value;
		//While we don't reach the end of the file
		while (!file.eof())
		{
			AudioStruct* tempAudio = new AudioStruct();
			file >> value;
			tempAudio->type = GetSoundType(value);
			file >> value;
			tempAudio->path = value;
			file >> value;
			switch (tempAudio->type)
			{
			case MUSIC:
			case AMBIENT:
				tempAudio->music = new sf::Music();
				tempAudio->music->openFromFile(tempAudio->path);
				if (value == "TRUE") tempAudio->music->setLoop(true);
				else tempAudio->music->setLoop(false);
				break;
			case INTERACTIONS:
			case UI:
				tempAudio->sound = new sf::Sound();
				tempAudio->buffer = new sf::SoundBuffer();
				tempAudio->buffer->loadFromFile(tempAudio->path);
				if (value == "TRUE") tempAudio->sound->setLoop(true);
				else tempAudio->sound->setLoop(false);
				tempAudio->sound->setBuffer(*tempAudio->buffer);
				break;
			}
			file >> value;
			size_t hash = hasher(value);
			tempAudio->id = hash;

			audioList.insert({ hash,tempAudio });
		}
		file.close();
	}
}

void SoundManager::Update()
{
	for (int i = (int)playingAudios.size()-1; i >= 0; i--)
	{
		//Delete finished audios else change audios volume
		if (playingAudios[i]->sound != nullptr)
		{
			if (playingAudios[i]->sound->getStatus() == sf::SoundSource::Stopped) 
			{
				delete playingAudios[i]->sound;
				playingAudios[i]->sound = nullptr;
				delete playingAudios[i];
				playingAudios[i] = nullptr;

				playingAudios.erase(playingAudios.begin() + i);
			}
			else if(!playingAudios[i]->manualVolume)
			{
				playingAudios[i]->sound->setVolume(GetVolume(playingAudios[i]->type));
			}
			else 
			{
				playingAudios[i]->manualVolume = false;
			}
		}
		else if (playingAudios[i]->music != nullptr)
		{
			if (playingAudios[i]->music->getStatus() == sf::SoundSource::Stopped) 
			{
				delete playingAudios[i]->music;
				playingAudios[i]->music = nullptr;
				delete playingAudios[i];
				playingAudios[i] = nullptr;
				playingAudios.erase(playingAudios.begin() + i);
			}
			else if(!playingAudios[i]->manualVolume)
			{
				playingAudios[i]->music->setVolume(GetVolume(playingAudios[i]->type));
			}
			else 
			{
				playingAudios[i]->manualVolume = false;
			}
		}
		playingAudios.shrink_to_fit();
	}
}



void SoundManager::ChangeState(GameStateType _state)
{
	state = _state;
	for (int i = 0; i < playingAudios.size(); i++)
	{
		if(playingAudios[i]->type == MUSIC || playingAudios[i]->type == AMBIENT) playingAudios[i]->music->setVolume(GetVolume(playingAudios[i]->type));
		else playingAudios[i]->sound->setVolume(GetVolume(playingAudios[i]->type));
	}
}

void SoundManager::SetVolumeAccordingToValue(std::string _name, float _value, float _offset)
{
	size_t hash = hasher(_name);
	float volume = _value;
	for (int i = 0; i < playingAudios.size(); i++)
	{
		if (playingAudios[i]->id == hash) 
		{
			playingAudios[i]->manualVolume = true;
			if( _value > 100) volume = _value  - ( _value - 100);

			volume -= _offset;
			if (volume < 0.f) volume = 0.0f;
			if (volume > m_volume) volume = m_volume;

			if (playingAudios[i]->type == MUSIC || playingAudios[i]->type == AMBIENT) playingAudios[i]->music->setVolume(volume);
			else playingAudios[i]->sound->setVolume(volume);
		}
	}
}

const float SoundManager::GetVolume(SoundType _type)
{
	float dB = MIN_DB;
	switch (state)
	{
	case MENUS:
		switch(_type)
		{
		case MUSIC:
			dB = -10;
			break;
		case UI:
			dB = -4;
			break;
		case AMBIENT:
			dB = -8;
			break;
		}
		break;
	case GAME:
		switch (_type)
		{
		case MUSIC:
			dB = -10;
			break;
		case INTERACTIONS:
			dB = -6;
			break;
		case UI:
			dB = -8;
			break;
		case AMBIENT:
			dB = -10;
			break;
		}
		break;
	case DIALOGUE:
		switch (_type)
		{
		case MUSIC:
			dB = -6;
			break;
		case UI:
			dB = -4;
			break;
		case AMBIENT:
			dB = -6;
			break;
		}
		break;
	}
	float volume =  m_volume * (1-(MAX_DB - dB) / (MAX_DB - MIN_DB));
	return volume;
}

SoundManager::~SoundManager()
{
	for (int i = (int)playingAudios.size() - 1; i > 0; i--)
	{
		if (playingAudios[i]->sound != nullptr)
		{
			delete playingAudios[i]->sound;
			playingAudios[i]->sound = nullptr;
			delete playingAudios[i];
			playingAudios[i] = nullptr;

			playingAudios.erase(playingAudios.begin() + i);
		}
		else if (playingAudios[i]->music != nullptr)
		{
			delete playingAudios[i]->music;
			playingAudios[i]->music = nullptr;
			delete playingAudios[i];
			playingAudios[i] = nullptr;
			playingAudios.erase(playingAudios.begin() + i);
		}
	}
	playingAudios.shrink_to_fit();

	for (auto i = audioList.begin(); i != audioList.end(); i++)
	{
		if (i->second->music != nullptr) 
		{
			i->second->music->stop();
			delete i->second->music;
			i->second->music = nullptr;
		}
		if(i->second->sound != nullptr)
		{
			i->second->sound->stop();
			delete i->second->sound;
			i->second->sound = nullptr;
		}

		delete i->second;
		i->second = nullptr;
	}
	audioList.clear();
}
