// 【scenario_interpreter】 シナリオCSVを読み、条件分岐と各ゲームコマンドを実行する。
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
#include <vector>

// クラス宣言: 公開操作と内部リソースをまとめる。
class ScenarioInterpreter
{
public:
// 表示対象の全文メッセージを保持する。
    ScenarioInterpreter(CutinManage &cutinManage, SaveData &saveData, ItemWindow &itemWindow, SoundManager &soundManager, BackgroundManager &backgroundManager, CharacterManager &characterManager, MessageWindow &messageWindow, SelectionWindow &selectionWindow, CoroutineManager &manager);
    ~ScenarioInterpreter();
// シナリオファイルを先頭から読み、各コマンドを順に実行する。
    Task InterpretScenario(const std::string &file_path);
// CSV行をカンマ区切りのトークン列へ分解する。
    std::vector<std::string> ParseCSVTokens(const std::string &line);
// コマンド名と引数を解釈し、対応する状態遷移または待機処理を実行する。
    Task ExecuteCommand(const std::string &command, const std::vector<std::string> &args);

// タイトルへ戻る状態遷移を要求するかを示す。
    bool isRetuningToTitle = false;
// ゲーム終了を要求するかを示す。
    bool isQuitting = false;

private:
    CutinManage &cutinManage;
    SaveData &saveData;
    SoundManager &soundManager;
    BackgroundManager &backgroundManager;
    CharacterManager &characterManager;
// 表示対象の全文メッセージを保持する。
    MessageWindow &messageWindow;
    SelectionWindow &selectionWindow;
    CoroutineManager &coroutineManager;
    ItemWindow &itemWindow;
// スキップ中かを示し、コマンド実行を省略する判定に使う。
    bool isSkipping = false;
// 入れ子のIF判定ごとのスキップ状態を保持する。
    std::vector<bool> ifSkipStack;
};