// 【chara_manage】 立ち絵テクスチャの読み込み、表示切替、描画状態を管理する。
#pragma once

#include "coroutine_task.hpp"
#include "coroutine_manager.hpp"
#include "wait_for_seconds.hpp"
#include "wait_until.hpp"
#include "tween.hpp"
#include "graphic.hpp"
#include "raylib.h"
#include <string>
#include <vector>
#include "asset_manage.hpp"

// クラス宣言: 公開操作と内部リソースをまとめる。
class CharacterManager
{
public:
    CharacterManager();
// 表示状態に応じてフェードイン・フェードアウトを行う。引数showで表示可否を指定する。
    Task ToggleShow(CoroutineManager &manager, bool show);
    void QuickChangeCharacter(const std::string &filePath, float scale = 1.0f);
// テクスチャをプールへ登録し、既存なら既存インデックス、新規なら格納先インデックスを返す。
    int LoadAndPoolTexture(const std::string &filePath);
// 指定パスがプールに登録済みか検索し、見つかればインデックス、なければ-1を返す。
    int IsExistingTexture(const std::string &filePath) const;
// 現在の表示状態と透明度に応じて内容を描画する。
    void Graphic();
    void UnloadAllCharacterTextures();
    void LoadAllCharaTexture();

private:
// 読み込んだテクスチャの所有リソースを保持する。
    std::vector<Texture2D> texture_pool;
// 各テクスチャに対応するファイルパスを保持する。
    std::vector<std::string> texture_paths;
// 現在描画中または選択中のテクスチャ位置を示す。
    int current_texture_index = 0;
// 表示中かどうかを示し、描画・フェードの分岐に使う。
    bool is_visible = true;
// フェード処理の現在透明度を保持する。
    float characterAlpha = 0.0f;
// 立ち絵の表示拡大率を保持する。
    float characterScale = 1.0f;
};