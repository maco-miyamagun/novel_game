// 【sound_manage】 効果音とBGMの再生、フェード、リソース解放を管理する。
#pragma once

#include "coroutine_task.hpp"
#include "coroutine_manager.hpp"
#include "wait_for_seconds.hpp"
#include "wait_until.hpp"
#include "tween.hpp"
#include "raylib.h"
#include <string>
#include "asset_manage.hpp"

// クラス宣言: 公開操作と内部リソースをまとめる。
class SoundManager
{
public:
    SoundManager();
    ~SoundManager();
// 指定パスの効果音を読み込み、再生する。
    void PlaySoundEffect(const std::string &filePath);
// 現在のBGMを停止してから指定ファイルをフェード再生する。
    Task PlayBackgroundMusic(CoroutineManager &manager, const std::string &filePath);
// 再生中のBGMをフェードアウトして停止し、関連リソースを解放する。
    Task StopBackgroundMusic(CoroutineManager &manager);
// 経過時間とマウス操作から選択状態を更新する。引数deltaTimeは前フレームからの秒数。
    void Logic();
    void UnloadAllSoundFiles();

private:
// 再生中の効果音リソースを保持する。
    Sound soundEffect;
// 再生中のBGMリソースを保持する。
    Music backgroundMusic;
// 再生中のBGMリソースを保持する。
    unsigned char *backgroundMusicFileData = nullptr;
// BGMが再生中かを示す。
    bool isBackgroundMusicPlaying = false;
};