#pragma once
#include <unordered_map>
#include "raylib.h"

class AudioManager
{
public:
    static AudioManager& Instance()
    {
        static AudioManager instance; //Guaranteed to be initialized only once
        return instance;
    }
    void Update();
    void PlaySoundByName(const char* soundName);
    void PlayMusicByName(const char* musicName);
    void StopMusicByName(const char* musicName);
    void CreateSound(const char* path, const char* soundName);
    void CreateMusic(const char* path, const char* musicName);
    void DeleteSound(const char* soundName);
    void DeleteMusic(const char* musicName);
    void SetMusicLoopStatus(const char* musicName, bool isLoop);
    bool IsSoundPlayingNow(const char* soundName);
    bool IsMusicPlayingNow(const char* musicName);
private:

    //Private constructor to prevent instantiation
    AudioManager();
    //Destructor
    ~AudioManager();
    
    bool IsSoundLoaded(const char* soundName);
    bool IsMusicLoaded(const char* musicName);
    std::unordered_map<const char*, Sound> soundsData;
    std::unordered_map<const char*, Music> musicsData;


    
};

