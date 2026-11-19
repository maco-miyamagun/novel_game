// 【japanese_font】 日本語グリフを含むフォントを読み込む。
#pragma once
#include <string>
#include <raylib.h>
#include "asset_manage.hpp"

// 指定パスとサイズで日本語グリフを含むフォントを読み込む。
Font GetJapaneseFont(const std::string &fontPath, int fontSize);