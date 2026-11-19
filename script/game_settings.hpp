// 【game_settings】 画面・ウィンドウ・選択肢レイアウトの定数を提供する。
#pragma once
#include <string>

const int GAME_WIDTH = 2560;
const int GAME_HEIGHT = 1440;

const int WINDOW_WIDTH = 1100;
const int WINDOW_HEIGHT = 680;
const std::string GAME_TITLE = "【ノベルゲーム】「月が綺麗ですね」";

namespace SelectionWindowLayout
{
    constexpr float X = 0.0f;
    constexpr float Y = 80.0f;
    constexpr float WIDTH = 1500.0f;
    constexpr float HEIGHT = 1000.0f;
    constexpr float TITLE_Y = 350.0f;
    constexpr float TITLE_FONT_SIZE = 200.0f;
    constexpr float OPTION_WIDTH = 1200.0f;
    constexpr float OPTION_HEIGHT = 150.0f;
    constexpr float OPTION_GAP = 30.0f;
    constexpr float FIRST_OPTION_Y = 100.0f;
    constexpr float OPTION_FONT_SIZE = 52.0f;
    constexpr float OPTION_TEXT_SPACING = 2.0f;
}