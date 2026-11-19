// 【scenario_interpreter】 シナリオCSVを読み、条件分岐と各ゲームコマンドを実行する。
#include "scenario_interpreter.hpp"

ScenarioInterpreter::ScenarioInterpreter(CutinManage &cutinManage, SaveData &saveData, ItemWindow &itemWindow, SoundManager &soundManager, BackgroundManager &backgroundManager, CharacterManager &characterManager, MessageWindow &messageWindow, SelectionWindow &selectionWindow, CoroutineManager &manager) : cutinManage(cutinManage), saveData(saveData), itemWindow(itemWindow), soundManager(soundManager), backgroundManager(backgroundManager), characterManager(characterManager), messageWindow(messageWindow), selectionWindow(selectionWindow), coroutineManager(manager)
{
}

ScenarioInterpreter::~ScenarioInterpreter()
{
}

// シナリオファイルを先頭から読み、各コマンドを順に実行する。
Task ScenarioInterpreter::InterpretScenario(const std::string &file_path)
{
// 暗号化シナリオを復号した文字列バッファを返し、呼び出し側で解放する。
    char *file = LoadScenarioFromFileEncrypted(file_path.c_str());

    if (file == nullptr)
    {
        TraceLog(LOG_ERROR, "Failed to load scenario file: %s", file_path.c_str());
// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }

// ループ: 対象を順番に処理し、各要素の状態を確認する。
    for (char *line = strtok(file, "\n"); line != nullptr; line = strtok(nullptr, "\n"))
    {
        std::string commandLine(line);
        if (!commandLine.empty() && commandLine.back() == '\r')
        {
            commandLine.pop_back();
        }

        // コメント行をスキップ
        if (commandLine.empty() || commandLine[0] == '#')
        {
            continue;
        }

        if (isRetuningToTitle || isQuitting)
        {
            break;
        }

        auto tokens = ParseCSVTokens(commandLine);

        if (!tokens.empty())
        {
            std::string command = tokens[0];
            std::vector<std::string> args(tokens.begin() + 1, tokens.end());
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(ExecuteCommand(command, args));
        }
    }

    UnloadFileText(file);
// コルーチン終了: 現在の処理分岐から正常に抜ける。
    co_return;
}

// 行をカンマ区切りのトークン列へ分解する。
std::vector<std::string> ScenarioInterpreter::ParseCSVTokens(const std::string &line)
{
    std::vector<std::string> tokens;
// ループ: 対象を順番に処理し、各要素の状態を確認する。
    for (const auto &token : line | std::views::split(','))
    {
        tokens.emplace_back(token.begin(), token.end());
    }
    return tokens;
}

// コマンド名と引数を解釈し、対応する状態遷移または待機処理を実行する。
Task ScenarioInterpreter::ExecuteCommand(const std::string &command, const std::vector<std::string> &args)
{
    if (command == "$IF_END")
    {
        if (!ifSkipStack.empty())
        {
            isSkipping = ifSkipStack.back();
            ifSkipStack.pop_back();
        }
        else
        {
            isSkipping = false;
        }
// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }
    else if (command == "$IF")
    {
        bool parentSkipping = isSkipping;
        bool conditionMatched = false;

        if (args.size() >= 2)
        {
            std::string condition = args[0];
            std::string value = args[1];

            if (condition == "SELECTED_OPTION")
            {
// 選択された選択肢のインデックスを返す。
                int selectedOptionIndex = selectionWindow.GetSelectedOptionIndex();
                int targetOptionIndex = std::stoi(value);
                conditionMatched = (selectedOptionIndex == targetOptionIndex);
            }
            else if (condition == "FLAG" && args.size() >= 3)
            {
                std::string flagKey = value;
                bool expectedValue = (args[2] == "true");
                conditionMatched = (saveData.GetFlag(flagKey) == expectedValue);
            }
            else if (condition == "NUMERIC_FLAG_MIN" && args.size() >= 3)
            {
                std::string flagKey = value;
                int expectedValue = std::stoi(args[2]);
                conditionMatched = (saveData.GetNumericFlag(flagKey) >= expectedValue);
            }
            else if (condition == "NUMERIC_FLAG_MAX" && args.size() >= 3)
            {
                std::string flagKey = value;
                int expectedValue = std::stoi(args[2]);
                conditionMatched = (saveData.GetNumericFlag(flagKey) < expectedValue);
            }
        }

        ifSkipStack.push_back(isSkipping);
        isSkipping = parentSkipping || !conditionMatched;
// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }
    else if (isSkipping)
    {
// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }

    if (command == "MESSAGE_OPEN")
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await coroutineManager.Start(messageWindow.ToggleShow(coroutineManager, true));
    }
    else if (command == "MESSAGE_CLOSE")
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await coroutineManager.Start(messageWindow.ToggleShow(coroutineManager, false));
    }
    else if (command == "CHARACTER_SHOW")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
            std::string scaleStr = (args.size() >= 2) ? args[1] : "1.0";
            float scale = std::stof(scaleStr);
            characterManager.QuickChangeCharacter(filePath, scale);
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(characterManager.ToggleShow(coroutineManager, true));
        }
    }
    else if (command == "CHARACTER_HIDE")
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await coroutineManager.Start(characterManager.ToggleShow(coroutineManager, false));
    }
    else if (command == "CHARACTER_CHANGE")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
            std::string scaleStr = (args.size() >= 2) ? args[1] : "1.0";
            float scale = std::stof(scaleStr);
            characterManager.QuickChangeCharacter(filePath, scale);
        }
    }
    else if (command == "BACKGROUND_CHANGE")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
            std::string sceneName = args[1];
            backgroundManager.QuickChangeBackground(filePath, sceneName);
        }
    }
    else if (command == "BACKGROUND_HIDE")
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await coroutineManager.Start(backgroundManager.ToggleShow(coroutineManager, false));
    }
    else if (command == "BACKGROUND_SHOW")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
            std::string sceneName = args[1];
            backgroundManager.QuickChangeBackground(filePath, sceneName);
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(backgroundManager.ToggleShow(coroutineManager, true));
        }
    }
    else if (command == "CUTIN_OPEN")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(cutinManage.Open(coroutineManager, filePath));
        }
    }
    else if (command == "CUTIN_CLOSE")
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await coroutineManager.Start(cutinManage.Hide(coroutineManager));
    }
    else if (command == "MESSAGE")
    {
        if (args.size() == 2)
        {
            std::string speaker = args[0];
            std::string msg = args[1];
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(messageWindow.Message_Text_Type(speaker + ": " + msg));
        }
    }
    else if (command == "SELECTION")
    {
        if (args.size() >= 2)
        {
            std::string title = args[0];
            std::vector<std::string> options(args.begin() + 1, args.end());
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(messageWindow.ToggleShow(coroutineManager, false));
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(selectionWindow.ShowSelection(coroutineManager, title, options));
        }
    }
    else if (command == "SET_SELECTION_TITLE")
    {
        if (!args.empty())
        {
            bool isTitle = (args[0] == "true");
            selectionWindow.SetIsTitle(isTitle);
        }
    }
    else if (command == "PLAY_SE")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
            soundManager.PlaySoundEffect(filePath);
        }
    }
    else if (command == "PLAY_BGM")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(soundManager.PlayBackgroundMusic(coroutineManager, filePath));
        }
    }
    else if (command == "STOP_BGM")
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await coroutineManager.Start(soundManager.StopBackgroundMusic(coroutineManager));
    }
    else if (command == "ITEM_OPEN")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(messageWindow.ToggleShow(coroutineManager, false));
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await coroutineManager.Start(itemWindow.OpenWin(coroutineManager, filePath));
        }
    }
    else if (command == "SET_FLAG")
    {
        if (args.size() >= 2)
        {
            std::string key = args[0];
            bool value = (args[1] == "true");
            saveData.SetFlag(key, value);
        }
    }
    else if (command == "SET_NUM_FLAG")
    {
        if (args.size() >= 2)
        {
            std::string key = args[0];
            int value = std::stoi(args[1]);
            saveData.SetNumericFlag(key, value);
        }
    }
    else if (command == "INCREMENT_NUM_FLAG")
    {
        if (args.size() >= 2)
        {
            std::string key = args[0];
            int incrementValue = std::stoi(args[1]);
// 指定キーの数値フラグを返す。未登録時は0。
            int currentValue = saveData.GetNumericFlag(key);
            saveData.SetNumericFlag(key, currentValue + incrementValue);
        }
    }
    else if (command == "CLEAR_ALL_FLAGS")
    {
        saveData.ClearAllFlags();
    }
    else if (command == "CLEAR_ALL_NUMERIC_FLAGS")
    {
        saveData.ClearAllNumericFlags();
    }
    else if (command == "WAIT")
    {
        if (!args.empty())
        {
            float waitTime = std::stof(args[0]);
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await WaitForSeconds(waitTime);
        }
    }
    else if (command == "QUIT")
    {
        isQuitting = true;
// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }
    else if (command == "RETURN_TITLE")
    {
        isRetuningToTitle = true;
// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }
    else if (command == "WAIT_FOR_KEY")
    {
        if (!args.empty())
        {
            int key = std::stoi(args[0]);
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await WaitUntil([key]()
                               { return IsKeyPressed(key); });
        }
    }
    else if (command == "WAIT_FOR_MOUSE")
    {
        if (!args.empty())
        {
            int button = std::stoi(args[0]);
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
            co_await WaitUntil([button]()
                               { return IsMouseButtonPressed(button); });
        }
    }
    else if (command == "WAIT_FOR_TAP")
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await WaitUntil([this]()
                           { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                                    IsKeyPressed(KEY_SPACE) ||
                                    IsKeyPressed(KEY_ENTER); });
    }

// コルーチン終了: 現在の処理分岐から正常に抜ける。
    co_return;
}