// 【item_window】 アイテムウィンドウの表示、フェード、テクスチャ解放を管理する。
#include "item_window.hpp"

ItemWindow::ItemWindow()
{
    // 初期化処理
    texture_pool.reserve(10);
    current_texture_index = 0;
    is_visible = false;
    alpha = 0.0f;
}

// 指定画像を読み込み、フェードインしてウィンドウを表示する。
Task ItemWindow::OpenWin(CoroutineManager &manager, const std::string &filePath)
{
    const int index = LoadAndPoolTexture(filePath);
    current_texture_index = index;
    alpha = 0.0f;
    is_visible = true;

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
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

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
    co_await WaitUntil([this]()
                       { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                                IsKeyPressed(KEY_SPACE) ||
                                IsKeyPressed(KEY_ENTER); });

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
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

// テクスチャをプールへ登録し、既存なら既存インデックス、新規なら格納先インデックスを返す。
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
// ループ: 対象を順番に処理し、各要素の状態を確認する。
    for (const auto &texture : texture_pool)
    {
        UnloadTexture(texture);
    }
    texture_pool.clear();
    texture_paths.clear();
}