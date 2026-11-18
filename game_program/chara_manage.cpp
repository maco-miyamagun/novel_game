#include "chara_manage.hpp"

CharacterManager::CharacterManager()
{
    // 初期化処理
    texture_pool.reserve(10);
    texture_paths.reserve(10);
    current_texture_index = 0;
    is_visible = false;
    characterAlpha = 0.0f;

    LoadAllCharaTexture();
}

Task CharacterManager::ToggleShow(CoroutineManager &manager, bool show)
{
    // 表示/非表示を切り替える処理

    if (show)
    {
        is_visible = true;

        co_await manager.Start(Tween(
            manager,
            characterAlpha,
            1.0f,
            0.5f,
            EaseType::EASEIN,
            [this](float value)
            {
                characterAlpha = value;
            }));

        co_return;
    }

    co_await manager.Start(Tween(
        manager,
        characterAlpha,
        0.0f,
        0.5f,
        EaseType::EASEOUT,
        [this](float value)
        {
            characterAlpha = value;
        }));

    is_visible = false;
}

void CharacterManager::QuickChangeCharacter(const std::string &filePath, float scale)
{
    // すぐにキャラクターを変更する処理
    const int index = LoadAndPoolTexture(filePath);
    current_texture_index = index;
    characterScale = scale;
}

int CharacterManager::LoadAndPoolTexture(const std::string &filePath)
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
int CharacterManager::IsExistingTexture(const std::string &filePath) const
{
    int index = 0;

    for (const auto &path : texture_paths)
    {
        if (path == filePath)
        {
            return index; // 同じテクスチャが見つかった場合はインデックスを返す
        }
        index++;
    }
    return -1; // 同じテクスチャが見つからなかった場合は-1を返す
}

void CharacterManager::Graphic()
{
    // グラフィック表示処理

    if (!is_visible || texture_pool.empty())
        return;

    const Texture2D &currentTexture = texture_pool[current_texture_index];

    Graphic_Image(
        currentTexture,
        0, -currentTexture.height / 4.0f + 1 / characterScale * 250.0f,
        1.0f * characterScale, 0.0f,
        Fade(WHITE, characterAlpha));
}

void CharacterManager::UnloadAllCharacterTextures()
{
    for (const auto &texture : texture_pool)
    {
        UnloadTexture(texture);
    }
    texture_pool.clear();
    texture_paths.clear();
    current_texture_index = 0;
    is_visible = false;
    characterAlpha = 0.0f;
}

void CharacterManager::LoadAllCharaTexture()
{
    LoadAndPoolTexture("Chara/香久夜ちゃ立ち絵remake.png");
    LoadAndPoolTexture("Chara/香久夜ちゃ立ち絵remakeいたずら笑み.png");
    LoadAndPoolTexture("Chara/香久夜ちゃ立ち絵remake困り顔.png");
    LoadAndPoolTexture("Chara/香久夜ちゃ立ち絵remake照れ.png");
    LoadAndPoolTexture("Chara/香久夜ちゃ立ち絵remake笑顔.png");
    LoadAndPoolTexture("Chara/香久夜ちゃ立ち絵remake涙.png");
    LoadAndPoolTexture("Chara/香久夜ちゃ立ち絵remake大きく不安げ.png");
}