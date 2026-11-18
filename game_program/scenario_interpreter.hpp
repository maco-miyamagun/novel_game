#pragma once

#include <string>
#include <vector>
#include "raylib.h"
#include "coroutine_task.hpp"
#include "message_window.hpp"
#include "wait_for_seconds.hpp"
#include "wait_until.hpp"
#include "asset_manage.hpp"
#include "selection_window.hpp"
#include <ranges>
#include "background_manage.hpp"
#include "chara_manage.hpp"
#include "sound_manage.hpp"
#include "item_window.hpp"
#include "save_data.hpp"
#include "cutin_manage.hpp"

class ScenarioInterpreter
{
public:
    ScenarioInterpreter(CutinManage &cutinManage, SaveData &saveData, ItemWindow &itemWindow, SoundManager &soundManager, BackgroundManager &backgroundManager, CharacterManager &characterManager, MessageWindow &messageWindow, SelectionWindow &selectionWindow, CoroutineManager &manager);
    ~ScenarioInterpreter();
    Task InterpretScenario(const std::string &file_path);
    std::vector<std::string> ParseCSVTokens(const std::string &line);
    Task ExecuteCommand(const std::string &command, const std::vector<std::string> &args);

    bool isRetuningToTitle = false;
    bool isQuitting = false;

private:
    CutinManage &cutinManage;
    SaveData &saveData;
    SoundManager &soundManager;
    BackgroundManager &backgroundManager;
    CharacterManager &characterManager;
    MessageWindow &messageWindow;
    SelectionWindow &selectionWindow;
    CoroutineManager &coroutineManager;
    ItemWindow &itemWindow;
    bool isSkipping = false;
};