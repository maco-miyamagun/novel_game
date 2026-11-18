#include "mouse.hpp"
#include "game_settings.hpp"

Vector2 GetVirtualMousePosition()
{
    float scale = std::min(
        (float)GetScreenWidth() / GAME_WIDTH,
        (float)GetScreenHeight() / GAME_HEIGHT);

    float offsetX = (GetScreenWidth() - GAME_WIDTH * scale) / 2.0f;
    float offsetY = (GetScreenHeight() - GAME_HEIGHT * scale) / 2.0f;

    Vector2 mouse = GetMousePosition();

    return {
        (mouse.x - offsetX) / scale,
        (mouse.y - offsetY) / scale};
}