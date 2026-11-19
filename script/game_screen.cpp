// 【game_screen】 ゲーム解像度のレンダーターゲット生成と画面描画を行う。
#include "game_screen.hpp"

// 指定解像度のレンダーターゲットを生成する。
RenderTexture2D CreateGameRenderTexture(int width, int height)
{
    RenderTexture2D renderTexture = LoadRenderTexture(width, height);

    if (renderTexture.id == 0)
    {
        TraceLog(LOG_ERROR, "仮想画面の作成に失敗しました");
    }

    SetTextureFilter(
        renderTexture.texture,
        TEXTURE_FILTER_BILINEAR);

    return renderTexture;
}

// レンダーターゲットの内容をウィンドウへ合成する。
void RenderGameScreen(RenderTexture2D gameScreen)
{

    BeginDrawing();

    ClearBackground(BLACK);

    float scale = std::min(
        (float)GetScreenWidth() / GAME_WIDTH,
        (float)GetScreenHeight() / GAME_HEIGHT);

    float drawWidth = GAME_WIDTH * scale;
    float drawHeight = GAME_HEIGHT * scale;

    float offsetX =
        (GetScreenWidth() - drawWidth) / 2.0f;

    float offsetY =
        (GetScreenHeight() - drawHeight) / 2.0f;

    DrawTexturePro(
        gameScreen.texture,
        {0, 0,
         (float)GAME_WIDTH,
         -(float)GAME_HEIGHT},
        {offsetX,
         offsetY,
         drawWidth,
         drawHeight},
        {0, 0},
        0.0f,
        WHITE);

    EndDrawing();
}