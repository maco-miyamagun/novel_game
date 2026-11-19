// 【background_manage】 背景テクスチャの読み込み、表示切替、描画状態を管理する。
#include "background_manage.hpp"

BackgroundManager::BackgroundManager()
{
    // 初期化処理
    texture_pool.reserve(10);
    texture_paths.reserve(10);
    current_texture_index = 0;
    is_visible = false;
    backgroundAlpha = 0.0f;
    current_scene_name = "";
}

// 表示状態に応じてフェードイン・フェードアウトを行う。引数showがtrueなら表示、falseなら非表示にする。
Task BackgroundManager::ToggleShow(CoroutineManager &manager, bool show)
{
    // 表示/非表示を切り替える処理

    if (show)
    {
        is_visible = true;

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await manager.Start(Tween(
            manager,
            backgroundAlpha,
            1.0f,
            0.5f,
            EaseType::EASEIN,
            [this](float value)
            {
                backgroundAlpha = value;
            }));

// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
    co_await manager.Start(Tween(
        manager,
        backgroundAlpha,
        0.0f,
        0.5f,
        EaseType::EASEOUT,
        [this](float value)
        {
            backgroundAlpha = value;
        }));

    is_visible = false;
}

void BackgroundManager::QuickChangeBackground(const std::string &filePath, const std::string &scene_name)
{
    // すぐに背景を変更する処理
    const int index = LoadAndPoolTexture(filePath);
    current_texture_index = index;
    current_scene_name = scene_name; // 現在のシーン名を更新
}

// テクスチャをプールへ登録し、既存なら既存インデックス、新規なら格納先インデックスを返す。
int BackgroundManager::LoadAndPoolTexture(const std::string &filePath)
{
    const int existingIndex = IsExistingTexture(filePath);
    if (existingIndex != -1)
    {
        return existingIndex;
    }

    if (texture_pool.size() >= 10)
    {
        // プールがいっぱいの場合は古いテクスチャを削除する処理
        UnloadTexture(texture_pool.front());
        texture_pool.erase(texture_pool.begin());
        texture_paths.erase(texture_paths.begin());
        if (current_texture_index > 0)
        {
            --current_texture_index;
        }
    }

    // テクスチャを読み込んでプールに追加する処理
    texture_pool.push_back(LoadTextureFromFileEncrypted(filePath));
    texture_paths.push_back(filePath);

    return static_cast<int>(texture_pool.size() - 1); // 新しいテクスチャのインデックスを返す
}

// インデックスを返すように変更
int BackgroundManager::IsExistingTexture(const std::string &filePath) const
{
// ループ: 対象を順番に処理し、各要素の状態を確認する。
    for (size_t index = 0; index < texture_paths.size(); ++index)
    {
        if (texture_paths[index] == filePath)
        {
            return static_cast<int>(index);
        }
    }
    return -1;
}

// 現在の表示状態と透明度に応じて内容を描画する。
void BackgroundManager::Graphic()
{
    // グラフィック表示処理

    if (!is_visible || texture_pool.empty())
        return;

    const Texture2D &currentTexture = texture_pool[current_texture_index];

    Graphic_Image(
        currentTexture,
        0, 0,
        1.40f, 0.0f,
        Fade(WHITE, backgroundAlpha));
}

// 指定フォントを用いて現在の背景を描画する。
void BackgroundManager::GraphicFont(Font font)
{
    // グラフィック表示処理

    if (!is_visible || texture_pool.empty())
        return;

    Graphic_BorderedText(
        font,
        current_scene_name,
        -GAME_WIDTH / 2 + 50.0f, GAME_HEIGHT / 2 - 80.0f,
        130.0f * backgroundAlpha, 1.0f,
        Fade(WHITE, backgroundAlpha + 0.2f),
        Fade(BLACK, backgroundAlpha + 0.2f),
        false, true);
}

void BackgroundManager::UnloadAllBackgroundTextures()
{
// ループ: 対象を順番に処理し、各要素の状態を確認する。
    for (const Texture2D &texture : texture_pool)
    {
        UnloadTexture(texture);
    }
    texture_pool.clear();
    texture_paths.clear();
}