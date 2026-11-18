#include "raylib.h"
#include <algorithm>
#include <string>
#include <vector>
#include "utf8.hpp"
#include "game_settings.hpp"
#include "asset_manage.hpp"
#include "mouse.hpp"
#include "japanese_font.hpp"
#include "graphic.hpp"
#include "game_screen.hpp"
#include "message_window.hpp"
#include "coroutine_manager.hpp"
#include "wait_for_seconds.hpp"
#include "wait_until.hpp"
#include "scenario_interpreter.hpp"
#include "selection_window.hpp"
#include "chara_manage.hpp"
#include "background_manage.hpp"
#include "sound_manage.hpp"

Task NovelStory(CoroutineManager &coroutineManager, ScenarioInterpreter &interpreter)
{
    while (!WindowShouldClose() && !interpreter.isQuitting)
    {
        interpreter.isRetuningToTitle = false;

        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/title.csv"));
        if (interpreter.isRetuningToTitle)
        {
            continue;
        }

        // co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d01.csv"));
        // if (interpreter.isRetuningToTitle)
        // {
        //     continue;
        // }
        // co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d02.csv"));
        // if (interpreter.isRetuningToTitle)
        // {
        //     continue;
        // }
        // co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d03.csv"));
        // if (interpreter.isRetuningToTitle)
        // {
        //     continue;
        // }
        // co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d04.csv"));
        // if (interpreter.isRetuningToTitle)
        // {
        //     continue;
        // }
        // co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d05.csv"));
        // if (interpreter.isRetuningToTitle)
        // {
        //     continue;
        // }
        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d06.csv"));
        if (interpreter.isRetuningToTitle)
        {
            continue;
        }
        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d07.csv"));
    }
}

int main()
{
    // ウィンドウ設定
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    InitWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        GAME_TITLE.c_str());
    InitAudioDevice();
    SetTargetFPS(60);

    RenderTexture2D gameScreen =
        CreateGameRenderTexture(GAME_WIDTH, GAME_HEIGHT);

    ChangeToResourcesDirectory();

    Font font = GetJapaneseFont(
        "Font/NotoSansJP-Bold.ttf",
        300);

    CharacterManager characterManager;
    BackgroundManager backgroundManager;
    MessageWindow messageWindow;
    SelectionWindow selectionWindow;
    CoroutineManager coroutineManager;
    SoundManager soundManager;
    ItemWindow itemWindow;
    SaveData saveData;
    CutinManage cutinManage;
    ScenarioInterpreter scenarioInterpreter(cutinManage, saveData, itemWindow, soundManager, backgroundManager, characterManager, messageWindow, selectionWindow, coroutineManager);

    coroutineManager.Start(NovelStory(coroutineManager, scenarioInterpreter));

    // ======================================
    // ゲームループ
    // ======================================

    while (!WindowShouldClose() && !scenarioInterpreter.isQuitting)
    {
        float deltaTime = GetFrameTime();
        coroutineManager.Update();
        selectionWindow.Logic(deltaTime);
        soundManager.Logic();

        // ==================================
        // 仮想画面への描画
        // ==================================

        BeginTextureMode(gameScreen);
        ClearBackground(BLACK);
        backgroundManager.Graphic();
        characterManager.Graphic();
        backgroundManager.GraphicFont(font);
        itemWindow.Graphic();
        cutinManage.Graphic();
        messageWindow.Graphic(font);
        selectionWindow.Graphic(font);
        EndTextureMode();

        RenderGameScreen(gameScreen);
    }

    // ======================================
    // リソース解放
    // ======================================

    UnloadFont(font);
    characterManager.UnloadAllCharacterTextures();
    backgroundManager.UnloadAllBackgroundTextures();
    cutinManage.UnloadAllCutinTextures();
    UnloadRenderTexture(gameScreen);
    soundManager.UnloadAllSoundFiles();
    itemWindow.UnloadAllItemTextures();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
