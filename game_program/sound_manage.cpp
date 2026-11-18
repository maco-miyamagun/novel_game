#include "sound_manage.hpp"

SoundManager::SoundManager()
{
    soundEffect = {0};
    backgroundMusic = {0};
    isBackgroundMusicPlaying = false;
    backgroundMusicFileData = nullptr;
}

SoundManager::~SoundManager()
{
    if (backgroundMusicFileData)
    {
        AssetEncryptor::Free(backgroundMusicFileData);
    }
}

void SoundManager::PlaySoundEffect(const std::string &filePath)
{
    if (soundEffect.frameCount > 0)
    {
        UnloadSound(soundEffect);
    }
    soundEffect = LoadSEFromFileEncrypted(filePath);
    PlaySound(soundEffect);
}

Task SoundManager::PlayBackgroundMusic(CoroutineManager &manager, const std::string &filePath)
{
    co_await manager.Start(StopBackgroundMusic(manager));

    if (backgroundMusicFileData)
    {
        AssetEncryptor::Free(backgroundMusicFileData);
    }

    auto [music, file_data] = LoadMusicFromFileEncrypted(filePath);
    backgroundMusic = music;
    backgroundMusicFileData = file_data;
    backgroundMusic.looping = true;
    PlayMusicStream(backgroundMusic);
    isBackgroundMusicPlaying = true;
}

Task SoundManager::StopBackgroundMusic(CoroutineManager &manager)
{
    if (isBackgroundMusicPlaying)
    {
        co_await manager.Start(Tween(
            manager,
            1.0,
            0.0,
            0.5f,
            EaseType::EASEIN,
            [this](float value)
            {
                SetMusicVolume(backgroundMusic, value);
            }));

        UnloadMusicStream(backgroundMusic);
        backgroundMusic = {0};
        isBackgroundMusicPlaying = false;
    }
}

void SoundManager::Logic()
{
    if (isBackgroundMusicPlaying)
    {
        UpdateMusicStream(backgroundMusic);
    }
}

void SoundManager::UnloadAllSoundFiles()
{
    if (soundEffect.frameCount > 0)
    {
        UnloadSound(soundEffect);
        soundEffect = {0};
    }

    if (backgroundMusic.frameCount > 0)
    {
        UnloadMusicStream(backgroundMusic);
        backgroundMusic = {0};
    }
}