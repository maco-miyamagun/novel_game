// 【graphic】 画像、図形、テキストをゲーム画面へ描画する補助関数を提供する。
#include "graphic.hpp"

// テクスチャを指定位置・拡大率・回転・色で描画する。
void Graphic_Image(
    Texture2D texture,
    float x,
    float y,
    float scale,
    float rotation,
    Color tint)
{
    float drawWidth = texture.width * scale;
    float drawHeight = texture.height * scale;
    float centerX = x + GAME_WIDTH / 2.0f;
    float centerY = -y + GAME_HEIGHT / 2.0f;

    float pivotX = drawWidth / 2.0f;
    float pivotY = drawHeight / 2.0f;

    DrawTexturePro(
        texture,
        (Rectangle){0, 0, (float)texture.width, (float)texture.height},
        (Rectangle){centerX, centerY, drawWidth, drawHeight},
        (Vector2){pivotX, pivotY},
        rotation,
        tint);
}

// 指定矩形を塗りつぶしで描画する。
void Graphic_Rectangle(
    float x,
    float y,
    float width,
    float height,
    Color color)
{
    float centerX = x + GAME_WIDTH / 2.0f;
    float centerY = -y + GAME_HEIGHT / 2.0f;

    float pivotX = width / 2.0f;
    float pivotY = height / 2.0f;

    DrawRectanglePro(
        (Rectangle){centerX, centerY, width, height},
        (Vector2){pivotX, pivotY},
        0.0f,
        color);
}

// 指定矩形の輪郭を指定線幅で描画する。
void Graphic_RectangleLines(
    float x,
    float y,
    float width,
    float height,
    float weight,
    Color color)
{
    float centerX = x + GAME_WIDTH / 2.0f;
    float centerY = -y + GAME_HEIGHT / 2.0f;

    float pivotX = width / 2.0f;
    float pivotY = height / 2.0f;

    float offsetX = centerX - pivotX;
    float offsetY = centerY - pivotY;

    DrawRectangleLinesEx(
        (Rectangle){offsetX, offsetY, width, height},
        weight,
        color);
}

// 指定フォントと座標で文字列を描画し、必要に応じて中央揃えする。
void Graphic_Text(
    Font font,
    const std::string &text,
    float x,
    float y,
    float fontSize,
    float spacing,
    Color color,
    bool is_center_x, bool is_center_y)
{
    float pivotX = x + GAME_WIDTH / 2.0f;
    float pivotY = -y + GAME_HEIGHT / 2.0f;

    Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);

    if (is_center_x)
    {
        pivotX -= textSize.x / 2.0f;
    }
    if (is_center_y)
    {
        pivotY -= textSize.y / 2.0f;
    }

    DrawTextEx(
        font,
        text.c_str(),
        (Vector2){pivotX, pivotY},
        fontSize,
        spacing,
        color);
}

// 指定文字列を縁取り付きで描画し、必要に応じて中央揃えする。
void Graphic_BorderedText(
    Font font,
    const std::string &text,
    float x,
    float y,
    float fontSize,
    float spacing,
    Color textColor,
    Color borderColor,
    bool is_center_x,
    bool is_center_y)
{
    // Draw border
    Graphic_Text(
        font,
        text,
        x - 2, y - 2, fontSize, spacing, borderColor, is_center_x, is_center_y);
    Graphic_Text(
        font,
        text,
        x + 2, y - 2, fontSize, spacing, borderColor, is_center_x, is_center_y);
    Graphic_Text(
        font,
        text,
        x - 2, y + 2, fontSize, spacing, borderColor, is_center_x, is_center_y);
    Graphic_Text(
        font,
        text,
        x + 2, y + 2, fontSize, spacing, borderColor, is_center_x, is_center_y);

    // Draw main text
    Graphic_Text(
        font,
        text,
        x, y, fontSize, spacing, textColor, is_center_x, is_center_y);
}