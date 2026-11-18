#pragma once
#include <string>
#include <raylib.h>
#include "game_settings.hpp"

void Graphic_Image(
    Texture2D texture,
    float x,
    float y,
    float scale,
    float rotation,
    Color tint);

void Graphic_Rectangle(
    float x,
    float y,
    float width,
    float height,
    Color color);

void Graphic_RectangleLines(
    float x,
    float y,
    float width,
    float height,
    float weight,
    Color color);

void Graphic_Text(
    Font font,
    const std::string &text,
    float x,
    float y,
    float fontSize,
    float spacing,
    Color color,
    bool is_center_x = false,
    bool is_center_y = false);

void Graphic_BorderedText(
    Font font,
    const std::string &text,
    float x,
    float y,
    float fontSize,
    float spacing,
    Color color,
    Color borderColor,
    bool is_center_x = false,
    bool is_center_y = false);