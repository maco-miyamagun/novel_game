// 【graphic】 画像、図形、テキスト描画の補助関数を提供する。
#pragma once
#include <string>
#include <raylib.h>
#include "game_settings.hpp"

// テクスチャを指定位置・拡大率・回転・色で描画する。
void Graphic_Image(
    Texture2D texture,
    float x,
    float y,
    float scale,
    float rotation,
    Color tint);

// 指定矩形を塗りつぶしで描画する。
void Graphic_Rectangle(
    float x,
    float y,
    float width,
    float height,
    Color color);

// 指定矩形の輪郭を指定線幅で描画する。
void Graphic_RectangleLines(
    float x,
    float y,
    float width,
    float height,
    float weight,
    Color color);

// 指定フォントと座標で文字列を描画し、必要に応じて中央揃えする。
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

// 指定文字列を縁取り付きで描画し、必要に応じて中央揃えする。
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