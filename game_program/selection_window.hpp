#pragma once

#include <string>
#include <vector>
#include "raylib.h"
#include "japanese_font.hpp"
#include "utf8.hpp"
#include "mouse.hpp"
#include "graphic.hpp"
#include "coroutine_task.hpp"
#include "wait_for_seconds.hpp"
#include "wait_until.hpp"
#include "tween.hpp"
#include "game_settings.hpp"

class SelectionWindow
{
public:
    SelectionWindow();
    Task ShowSelection(CoroutineManager &manager, const std::string &title, const std::vector<std::string> &options);
    void OnMouseHover(const Vector2 &mousePosition, Rectangle optionRect, int optionIndex);
    void OnMouseClick(const Vector2 &mousePosition, Rectangle optionRect, int optionIndex);
    void Logic(float deltaTime);
    Task ToggleShow(CoroutineManager &manager, bool show);
    void Graphic(Font font);
    int GetSelectedOptionIndex();
    void SetIsTitle(bool isTitle) { is_title = isTitle; }

private:
    Rectangle GetOptionRect(size_t optionIndex) const;

    std::string title;
    std::vector<std::string> options;
    bool is_visible = false;
    int hoveredOptionIndex = -1;
    int clickedOptionIndex = -1;
    int selectedOptionIndex = 0;
    float windowAlpha = 0.0f;
    bool is_title = false;
};
