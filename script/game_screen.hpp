#pragma once
#include <string>
#include <raylib.h>
#include "game_settings.hpp"

RenderTexture2D CreateGameRenderTexture(int width, int height);
void RenderGameScreen(RenderTexture2D gameScreen);