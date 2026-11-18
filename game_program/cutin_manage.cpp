#include "cutin_manage.hpp"

CutinManage::CutinManage()
{
    // 初期化処理
    texture_pool.reserve(10);
    current_texture_index = 0;
    is_visible = false;
    alpha = 0.0f;
}

Task CutinManage::Open(CoroutineManager &manager, const std::string &filePath)
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
}

Task CutinManage::Hide(CoroutineManager &manager)
{
    if (!is_visible)
    {
        co_return;
    }

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

int CutinManage::LoadAndPoolTexture(const std::string &filePath)
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
int CutinManage::IsExistingTexture(const std::string &filePath) const
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

void CutinManage::Graphic()
{
    if (!is_visible || texture_pool.empty())
        return;

    const Texture2D &currentTexture = texture_pool[current_texture_index];

    Graphic_Image(
        currentTexture,
        0, 0,
        0.8f, 0.0f,
        Fade(WHITE, alpha));
}

void CutinManage::UnloadAllCutinTextures()
{
    for (const auto &texture : texture_pool)
    {
        UnloadTexture(texture);
    }
    texture_pool.clear();
    texture_paths.clear();
}