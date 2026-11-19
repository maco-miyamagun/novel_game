// 【chara_manage】 立ち絵テクスチャの読み込み、表示切替、描画状態を管理する。
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

// 表示状態に応じてフェードイン・フェードアウトを行う。引数showがtrueなら表示、falseなら非表示にする。
Task CharacterManager::ToggleShow(CoroutineManager &manager, bool show)
{
    // 表示/非表示を切り替える処理

    if (show)
    {
        is_visible = true;

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
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

// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
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

// テクスチャをプールへ登録し、既存なら既存インデックス、新規なら格納先インデックスを返す。
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

// ループ: 対象を順番に処理し、各要素の状態を確認する。
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

// 現在の表示状態と透明度に応じて内容を描画する。
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
// ループ: 対象を順番に処理し、各要素の状態を確認する。
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