//CppCon 2017: Guy Somberg “Game Audio Programming in C++”
//https://www.youtube.com/watch?v=M8Bd7uHH4Yg

#include "fmod.hpp"
#include <iostream>
#include <map>
#include <vector>
#pragma comment(lib,"fmod_vc.lib")

struct Vec3
{
	float x, y, z;
};

//サウンドシステムのデータストア
struct Implementation
{
	FMOD::System* fmodSystem;
	int nextChannelId=0;
	typedef std::map<std::string, FMOD::Sound*> SoundMap;
	typedef std::map<int, FMOD::Channel*> ChannelMap;
	SoundMap sounds;
	ChannelMap channels;
	Implementation()
	{
		FMOD::System_Create(&fmodSystem);
		fmodSystem->init(32, FMOD_INIT_NORMAL, nullptr);
	}
	~Implementation()
	{

	}
	void Update()
	{
		//停止状態のチャンネルが入る
		std::vector<ChannelMap::iterator> stoppendChannels;

		for (auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it)
		{
			bool bisPlaying = false;
			it->second->isPlaying(&bisPlaying);
			if (!bisPlaying)
			{
				stoppendChannels.push_back(it);
			}
		}
		for (auto& it : stoppendChannels)
		{
			//停止しているものは削除
			channels.erase(it);
		}
		fmodSystem->update();
	}
};

Implementation* implementation = nullptr;

class AudioEngine
{
public:
	static void Init()
	{
		implementation = new Implementation;
	}
	static void Update()
	{
		implementation->Update();
	}
	static void Shutdown()
	{
		delete implementation;
	}
	void LoadSound(const std::string& soundName, bool is3D = true, bool isLooping = false, bool isStream = false)
	{
		auto foundId = implementation->sounds.find(soundName);
		if (foundId != implementation->sounds.end()) { return; }
		FMOD_MODE mode = FMOD_DEFAULT;
		mode |= is3D ? FMOD_3D : FMOD_2D;
		mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
		FMOD::Sound* sound = nullptr;
		FMOD_RESULT a = implementation->fmodSystem->createSound(soundName.c_str(), mode, nullptr, &sound);
		if (sound)
		{
			implementation->sounds[soundName] = sound;
		}
	}
	void UnLoadSound(const std::string& soundName)
	{
		auto foundIt = implementation->sounds.find(soundName);
		if (foundIt == implementation->sounds.end()) { return; }

		foundIt->second->release();
		implementation->sounds.erase(foundIt);
	}
	void Set3dListenerAndOrientation(const Vec3& pos, const Vec3& look, const Vec3& up);
	int  PlaySound(const std::string& soundName, const Vec3& pos = Vec3{ 0,0,0 }, float volumedB = 0.f)
	{
		int channelId = implementation->nextChannelId++;
		auto foundIt = implementation->sounds.find(soundName);
		if (foundIt == implementation->sounds.end())
		{
			LoadSound(soundName);
			foundIt = implementation->sounds.find(soundName);
			if (foundIt == implementation->sounds.end())
			{
				return channelId;
			}
		}
		FMOD::Channel* channel = nullptr;
		FMOD_RESULT a = implementation->fmodSystem->playSound(foundIt->second, nullptr, true, &channel);
		if (channel)
		{
			FMOD_VECTOR position{ pos.x, pos.y, pos.z };
			channel->set3DAttributes(&position, nullptr);
			channel->setVolume(volumedB);
			channel->setPaused(false);
			implementation->channels[channelId] = channel;
		}
		return channelId;
	}
	void StopChannnel(int channelId);
	void PauseChannnel(int channelId);
	void StopAllChannnels();
	void SetChannel3dPosition(int channelId, const Vec3& pos);
	void SetChannelVolume(int channelId, float volumedB);
	bool IsPlaying(int channelId) const;
	// Add more functions....
};

int main()
{
	AudioEngine e;
	e.Init();
	//e.LoadSound("lastcastle.ogg");
	e.PlaySound("lastcastle.ogg", Vec3{ 2,0,0 }, 1);
	while (1)
	{
		e.Update();
	
	}
	e.Shutdown();
//	FMOD::System* system = nullptr;
//	FMOD::System_Create(&system);
//	system->init(32, FMOD_INIT_NORMAL, nullptr);
//	FMOD::Sound* sound = nullptr;
//	system->createSound("lastcastle.ogg",FMOD_DEFAULT,nullptr,&sound);
//	FMOD::Channel* channel = nullptr;
//	system->playSound(sound,nullptr,false,&channel);
//
//	bool isPlay = true;
//	while (isPlay)
//	{
//		channel->isPlaying(&isPlay);
//		system->update();
//	}
}