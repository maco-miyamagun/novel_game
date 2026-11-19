// 【cutin_manage】 カットイン画像の表示、フェード、テクスチャ解放を管理する。
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
#include "game_settings.hpp"

// クラス宣言: 公開操作と内部リソースをまとめる。
class CutinManage
{
public:
    CutinManage();
// 指定画像を読み込み、フェードインして表示する。
    Task Open(CoroutineManager &manager, const std::string &filePath);
// 表示中ならフェードアウトして非表示へ遷移する。
    Task Hide(CoroutineManager &manager);
// テクスチャをプールへ登録し、既存なら既存インデックス、新規なら格納先インデックスを返す。
    int LoadAndPoolTexture(const std::string &filePath);
// 指定パスがプールに登録済みか検索し、見つかればインデックス、なければ-1を返す。
    int IsExistingTexture(const std::string &filePath) const;
// 現在の表示状態と透明度に応じて内容を描画する。
    void Graphic();
    void UnloadAllCutinTextures();

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
    float alpha = 0.0f;
};