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

// ゲーム全体のシナリオ進行をコルーチンとして管理する。
// 各シナリオは完了まで待機し、タイトルへ戻る指示が出た場合は次の章を実行せず先頭へ戻る。
Task NovelStory(CoroutineManager &coroutineManager, ScenarioInterpreter &interpreter)
{
    // 最初に注意画面を表示し、ユーザー操作が完了するまでゲーム本編の進行を開始しない。
    co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/warning.csv"));

    // ウィンドウが閉じられるか、シナリオ側で終了フラグが立つまで本編を繰り返し進行する。
    while (!WindowShouldClose() && !interpreter.isQuitting)
    {
        // タイトルへ戻った直後でも前回の状態が残らないよう、章切替前にフラグを初期化する。
        interpreter.isRetuningToTitle = false;

        // タイトルシーンを終えた後、以下の章へ順番に進む。
        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/title.csv"));
        if (interpreter.isRetuningToTitle)
        {
            continue;
        }

        // 複数の章を個別に実行し、章ごとの終了理由を確認してタイトルへ戻れるようにする。
        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d01.csv"));
        if (interpreter.isRetuningToTitle)
        {
            continue;
        }
        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d02.csv"));
        if (interpreter.isRetuningToTitle)
        {
            continue;
        }
        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d03.csv"));
        if (interpreter.isRetuningToTitle)
        {
            continue;
        }
        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d04.csv"));
        if (interpreter.isRetuningToTitle)
        {
            continue;
        }
        co_await coroutineManager.Start(interpreter.InterpretScenario("ScenarioScript/d05.csv"));
        if (interpreter.isRetuningToTitle)
        {
            continue;
        }
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
    // ウィンドウ、オーディオ、フレームレートをゲーム表示に必要な初期状態へ設定する。
    // リサイズと高DPIを有効にし、端末の解像度差があっても仮想画面を使って描画できるようにする。
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    InitWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        GAME_TITLE.c_str());
    InitAudioDevice();
    SetTargetFPS(60);

    // ゲーム画面を固定解像度の仮想テクスチャへ描画し、最後にウィンドウへ拡大縮小して表示する。
    RenderTexture2D gameScreen =
        CreateGameRenderTexture(GAME_WIDTH, GAME_HEIGHT);

    // 暗号化されたリソースを読み込めるよう、現在の作業ディレクトリをResourcesへ移動する。
    ChangeToResourcesDirectory();

    // 日本語表示に使用するフォントを読み込み、全てのUI描画で共有する。
    Font font = GetJapaneseFont(
        "Font/NotoSansJP-Bold.ttf",
        300);

    // 各マネージャーは表示内容やリソースの所有範囲を分け、シナリオ解釈から操作できるよう参照渡しする。
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

    // NovelStoryをコルーチンとして開始し、メインループの更新処理を通して段階的に再開する。
    coroutineManager.Start(NovelStory(coroutineManager, scenarioInterpreter));

    // ======================================
    // メインループ: 入力更新、コルーチン進行、描画を毎フレーム実行する。
    // ======================================

    while (!WindowShouldClose() && !scenarioInterpreter.isQuitting)
    {
        // 前フレームからの経過秒数を基に、フェードや選択肢の経過状態を更新する。
        float deltaTime = GetFrameTime();

        // 待機中のタスクが完了条件を満たしたか確認し、再開可能な処理だけを進める。
        coroutineManager.Update();
        selectionWindow.Logic(deltaTime);
        soundManager.Logic();

        // ==================================
        // 仮想画面への描画: 奥側から手前側へ重ね順を揃えて描画する。
        // ==================================

        BeginTextureMode(gameScreen);
        ClearBackground(BLACK);
        // 背景、立ち絵、アイテム、カットイン、本文、選択肢の順で1画面へ合成する。
        backgroundManager.Graphic();
        characterManager.Graphic();
        backgroundManager.GraphicFont(font);
        itemWindow.Graphic();
        cutinManage.Graphic();
        messageWindow.Graphic(font);
        selectionWindow.Graphic(font);
        EndTextureMode();

        // 仮想画面をウィンドウサイズへフィットさせ、レターボックス等を含めて実際に表示する。
        RenderGameScreen(gameScreen);
    }

    // ======================================
    // 終了処理: 全てのGPUリソース、オーディオ、ウィンドウを逆順に近い形で解放する。
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
