#include "fmod.hpp"
#include <iostream>
#pragma comment(lib,"fmod_vc.lib")
int main()
{
	FMOD::System* system = nullptr;
	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, nullptr);
	FMOD::Sound* sound = nullptr;
	system->createSound("lastcastle.ogg",FMOD_DEFAULT,nullptr,&sound);
	FMOD::Channel* channel = nullptr;
	system->playSound(sound,nullptr,false,&channel);

	bool isPlay = true;
	while (isPlay)
	{
		channel->isPlaying(&isPlay);
		system->update();
	}
}