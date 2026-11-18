#pragma once

#include "coroutine_task.hpp"
#include "coroutine_manager.hpp"
#include "wait_for_seconds.hpp"
#include "wait_until.hpp"
#include "tween.hpp"
#include "raylib.h"
#include <string>
#include "asset_manage.hpp"

class SoundManager
{
public:
    SoundManager();
    ~SoundManager();
    void PlaySoundEffect(const std::string &filePath);
    Task PlayBackgroundMusic(CoroutineManager &manager, const std::string &filePath);
    Task StopBackgroundMusic(CoroutineManager &manager);
    void Logic();
    void UnloadAllSoundFiles();

private:
    Sound soundEffect;
    Music backgroundMusic;
    unsigned char *backgroundMusicFileData = nullptr;
    bool isBackgroundMusicPlaying = false;
};