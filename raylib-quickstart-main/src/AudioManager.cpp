#include "AudioManager.h"
AudioManager::AudioManager() {
	InitAudioDevice();
}
void AudioManager::Update()
{
	for (auto& music : musicsData)
	{
		if (IsMusicStreamPlaying(music.second)) {
			UpdateMusicStream(music.second);
		}
	}
}

void AudioManager::PlaySoundByName(const char* soundName)
{
	if (!IsSoundLoaded(soundName))
	{
		printf("Sound File Not Loaded");
		return;
	}
	
	PlaySound(soundsData[soundName]);
}

void AudioManager::PlayMusicByName(const char* musicName)
{
	if (!IsMusicLoaded(musicName))
	{
		printf("Sound File Not Loaded");
		return;
	}
	PlayMusicStream(musicsData[musicName]);
}

void AudioManager::StopMusicByName(const char* musicName)
{
	if (!IsMusicLoaded(musicName))
	{
		printf("Music File Not Loaded");
		return;
	}
	StopMusicStream(musicsData[musicName]);

}

void AudioManager::DeleteSound(const char* soundName)
{
	if (!IsSoundLoaded(soundName))
	{
		printf("Sound File Not Loaded");
		return;
	}
	UnloadSound(soundsData[soundName]);
	soundsData.erase(soundName);
}

void AudioManager::DeleteMusic(const char* musicName)
{
	if (!IsMusicLoaded(musicName))
	{
		printf("Music File Not Loaded");
		return;
	}
	UnloadMusicStream(musicsData[musicName]);
	musicsData.erase(musicName);
}

void AudioManager::SetMusicLoopStatus(const char* musicName,bool isLoop)
{
	if (!IsMusicLoaded(musicName))
	{
		printf("Music File Not Loaded");
		return;
	}
	musicsData[musicName].looping = isLoop;
}
bool AudioManager::IsSoundPlayingNow(const char* soundName)
{
	if (!IsSoundLoaded(soundName))
	{
		printf("Sound File Not Loaded");
		return false;
	}
	return IsSoundPlaying(soundsData[soundName]);
}
bool AudioManager::IsMusicPlayingNow(const char* musicName)
{
	if (!IsMusicLoaded(musicName))
	{
		printf("Music File Not Loaded");
		return false;
	}
	return IsMusicStreamPlaying(musicsData[musicName]);
}


void AudioManager::CreateSound(const char* path, const char* soundName)
{
	if (IsSoundLoaded(soundName))
	{
		printf("Sound File Already Loaded");
		return;
	}

	
	soundsData[soundName] = LoadSound(path);
}

void AudioManager::CreateMusic(const char* path, const char* musicName)
{
	if (IsMusicLoaded(musicName))
	{
		printf("Music File Already Loaded");
		return;
	}


	musicsData[musicName] = LoadMusicStream(path);
}


bool AudioManager::IsSoundLoaded(const char* soundName)
{
	return soundsData.find(soundName) != soundsData.end();
}

bool AudioManager::IsMusicLoaded(const char* musicName)
{
	return musicsData.find(musicName) != musicsData.end();
}

AudioManager::~AudioManager()
{

	for (; soundsData.size() != 0;)
	{
		DeleteSound(soundsData.begin()->first);
	}
	soundsData.clear();
	for (; musicsData.size() != 0;)
	{
		DeleteMusic(musicsData.begin()->first);
	}
	musicsData.clear();
	
	CloseAudioDevice();
}
