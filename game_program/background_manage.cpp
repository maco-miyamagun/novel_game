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

Task BackgroundManager::ToggleShow(CoroutineManager &manager, bool show)
{
    // 表示/非表示を切り替える処理

    if (show)
    {
        is_visible = true;

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

        co_return;
    }

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
    for (size_t index = 0; index < texture_paths.size(); ++index)
    {
        if (texture_paths[index] == filePath)
        {
            return static_cast<int>(index);
        }
    }
    return -1;
}

void BackgroundManager::Graphic(Font font)
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
    for (const Texture2D &texture : texture_pool)
    {
        UnloadTexture(texture);
    }
    texture_pool.clear();
    texture_paths.clear();
}