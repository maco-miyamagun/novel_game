// 【sound_manage】 効果音とBGMの再生、フェード、リソース解放を管理する。
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

// 指定パスの効果音を読み込み、再生する。
void SoundManager::PlaySoundEffect(const std::string &filePath)
{
    if (soundEffect.frameCount > 0)
    {
        UnloadSound(soundEffect);
    }
    soundEffect = LoadSEFromFileEncrypted(filePath);
    PlaySound(soundEffect);
}

// 現在のBGMを停止してから指定ファイルをフェード再生する。
Task SoundManager::PlayBackgroundMusic(CoroutineManager &manager, const std::string &filePath)
{
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
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

// 再生中のBGMをフェードアウトして停止し、関連リソースを解放する。
Task SoundManager::StopBackgroundMusic(CoroutineManager &manager)
{
    if (isBackgroundMusicPlaying)
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
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

// 経過時間とマウス操作から選択状態を更新する。引数deltaTimeは前フレームからの秒数。
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