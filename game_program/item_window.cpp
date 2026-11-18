#include "item_window.hpp"

ItemWindow::ItemWindow()
{
    // 初期化処理
    texture_pool.reserve(10);
    current_texture_index = 0;
    is_visible = false;
    alpha = 0.0f;
}

Task ItemWindow::OpenWin(CoroutineManager &manager, const std::string &filePath)
{
    const int index = LoadAndPoolTexture(filePath);
    current_texture_index = index;
    alpha = 0.0f;
    is_visible = true;

    co_await manager.Start(Tween(
        manager,
        alpha,
        1.0f,
        0.5f,
        EaseType::EASEIN,
        [this](float value)
        {
            alpha = value;
        }));

    co_await WaitUntil([this]()
                       { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                                IsKeyPressed(KEY_SPACE) ||
                                IsKeyPressed(KEY_ENTER); });

    co_await manager.Start(Tween(
        manager,
        alpha,
        0.0f,
        0.5f,
        EaseType::EASEOUT,
        [this](float value)
        {
            alpha = value;
        }));

    is_visible = false;
}

int ItemWindow::LoadAndPoolTexture(const std::string &filePath)
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
int ItemWindow::IsExistingTexture(const std::string &filePath) const
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

void ItemWindow::Graphic()
{
    if (!is_visible || texture_pool.empty())
        return;

    float base_scale = alpha;

    const Texture2D &currentTexture = texture_pool[current_texture_index];

    Graphic_Rectangle(
        0, 0,
        GAME_WIDTH, GAME_HEIGHT,
        Fade(BLACK, 0.82f * alpha));

    Graphic_Image(
        currentTexture,
        0, 0,
        0.8f * base_scale, 0.0f,
        Fade(WHITE, alpha));
}

void ItemWindow::UnloadAllItemTextures()
{
    for (const auto &texture : texture_pool)
    {
        UnloadTexture(texture);
    }
    texture_pool.clear();
    texture_paths.clear();
}