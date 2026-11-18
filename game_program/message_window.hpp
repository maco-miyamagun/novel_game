#pragma once

#include <string>
#include <vector>
#include "raylib.h"
#include "japanese_font.hpp"
#include "utf8.hpp"
#include "graphic.hpp"
#include "coroutine_task.hpp"
#include "wait_for_seconds.hpp"
#include "wait_until.hpp"
#include "tween.hpp"

class MessageWindow
{
public:
    MessageWindow();
    Task Message_Text_Type(const std::string &msg);
    Task ToggleShow(CoroutineManager &manager, bool show);
    void ShowAllText();
    void ClearMessage();
    void Graphic(Font font);

private:
    std::string message;
    int displayedCharacters;
    float textSpeed;
    bool is_visible = true;
    float windowAlpha = 0.0f;
};