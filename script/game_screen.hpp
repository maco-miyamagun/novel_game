// 【game_screen】 ゲーム解像度のレンダーターゲット生成と描画を行う。
#pragma once
#include <string>
#include <raylib.h>
#include "game_settings.hpp"

// 指定解像度のレンダーターゲットを生成する。
RenderTexture2D CreateGameRenderTexture(int width, int height);
// レンダーターゲットの内容をウィンドウへ合成する。
void RenderGameScreen(RenderTexture2D gameScreen);