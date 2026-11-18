#include "scenario_interpreter.hpp"

ScenarioInterpreter::ScenarioInterpreter(CutinManage &cutinManage, SaveData &saveData, ItemWindow &itemWindow, SoundManager &soundManager, BackgroundManager &backgroundManager, CharacterManager &characterManager, MessageWindow &messageWindow, SelectionWindow &selectionWindow, CoroutineManager &manager) : cutinManage(cutinManage), saveData(saveData), itemWindow(itemWindow), soundManager(soundManager), backgroundManager(backgroundManager), characterManager(characterManager), messageWindow(messageWindow), selectionWindow(selectionWindow), coroutineManager(manager)
{
}

ScenarioInterpreter::~ScenarioInterpreter()
{
}

Task ScenarioInterpreter::InterpretScenario(const std::string &file_path)
{
    char *file = LoadScenarioFromFileEncrypted(file_path.c_str());

    if (file == nullptr)
    {
        TraceLog(LOG_ERROR, "Failed to load scenario file: %s", file_path.c_str());
        co_return;
    }

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
            co_await coroutineManager.Start(ExecuteCommand(command, args));
        }
    }

    UnloadFileText(file);
    co_return;
}

std::vector<std::string> ScenarioInterpreter::ParseCSVTokens(const std::string &line)
{
    std::vector<std::string> tokens;
    for (const auto &token : line | std::views::split(','))
    {
        tokens.emplace_back(token.begin(), token.end());
    }
    return tokens;
}

Task ScenarioInterpreter::ExecuteCommand(const std::string &command, const std::vector<std::string> &args)
{
    if (command == "$IF_END")
    {
        isSkipping = false;
    }
    else if (isSkipping)
    {
        co_return;
    }

    if (command == "MESSAGE_OPEN")
    {
        co_await coroutineManager.Start(messageWindow.ToggleShow(coroutineManager, true));
    }
    else if (command == "MESSAGE_CLOSE")
    {
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
            co_await coroutineManager.Start(characterManager.ToggleShow(coroutineManager, true));
        }
    }
    else if (command == "CHARACTER_HIDE")
    {
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
            backgroundManager.QuickChangeBackground(filePath);
        }
    }
    else if (command == "BACKGROUND_HIDE")
    {
        co_await coroutineManager.Start(backgroundManager.ToggleShow(coroutineManager, false));
    }
    else if (command == "BACKGROUND_SHOW")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
            std::string sceneName = args[1];
            backgroundManager.QuickChangeBackground(filePath, sceneName);
            co_await coroutineManager.Start(backgroundManager.ToggleShow(coroutineManager, true));
        }
    }
    else if (command == "CUTIN_OPEN")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
            co_await coroutineManager.Start(cutinManage.Open(coroutineManager, filePath));
        }
    }
    else if (command == "CUTIN_CLOSE")
    {
        co_await coroutineManager.Start(cutinManage.Hide(coroutineManager));
    }
    else if (command == "MESSAGE")
    {
        if (args.size() == 2)
        {
            std::string speaker = args[0];
            std::string msg = args[1];
            co_await coroutineManager.Start(messageWindow.Message_Text_Type(speaker + ": " + msg));
        }
    }
    else if (command == "SELECTION")
    {
        if (args.size() >= 2)
        {
            std::string title = args[0];
            std::vector<std::string> options(args.begin() + 1, args.end());
            co_await coroutineManager.Start(messageWindow.ToggleShow(coroutineManager, false));
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
            co_await coroutineManager.Start(soundManager.PlayBackgroundMusic(coroutineManager, filePath));
        }
    }
    else if (command == "STOP_BGM")
    {
        co_await coroutineManager.Start(soundManager.StopBackgroundMusic(coroutineManager));
    }
    else if (command == "ITEM_OPEN")
    {
        if (!args.empty())
        {
            std::string filePath = args[0];
            co_await coroutineManager.Start(messageWindow.ToggleShow(coroutineManager, false));
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
    else if (command == "$IF")
    {
        if (args.size() >= 2)
        {
            std::string condition = args[0];
            std::string value = args[1];

            if (condition == "SELECTED_OPTION" && !args.empty())
            {
                int selectedOptionIndex = selectionWindow.GetSelectedOptionIndex();
                int targetOptionIndex = std::stoi(value);

                isSkipping = (selectedOptionIndex != targetOptionIndex);
            }

            if (condition == "FLAG" && args.size() >= 3)
            {
                std::string flagKey = value;
                bool expectedValue = (args[2] == "true");

                bool actualValue = saveData.GetFlag(flagKey);
                isSkipping = (actualValue != expectedValue);
            }

            if (condition == "NUMERIC_FLAG_MIN" && args.size() >= 3)
            {
                std::string flagKey = value;
                int expectedValue = std::stoi(args[2]);

                int actualValue = saveData.GetNumericFlag(flagKey);
                isSkipping = (actualValue < expectedValue);
            }

            if (condition == "NUMERIC_FLAG_MAX" && args.size() >= 3)
            {
                std::string flagKey = value;
                int expectedValue = std::stoi(args[2]);

                int actualValue = saveData.GetNumericFlag(flagKey);
                isSkipping = (actualValue >= expectedValue);
            }
        }
    }
    else if (command == "WAIT")
    {
        if (!args.empty())
        {
            float waitTime = std::stof(args[0]);
            co_await WaitForSeconds(waitTime);
        }
    }
    else if (command == "QUIT")
    {
        isQuitting = true;
        co_return;
    }
    else if (command == "RETURN_TITLE")
    {
        isRetuningToTitle = true;
        co_return;
    }
    else if (command == "WAIT_FOR_KEY")
    {
        if (!args.empty())
        {
            int key = std::stoi(args[0]);
            co_await WaitUntil([key]()
                               { return IsKeyPressed(key); });
        }
    }
    else if (command == "WAIT_FOR_MOUSE")
    {
        if (!args.empty())
        {
            int button = std::stoi(args[0]);
            co_await WaitUntil([button]()
                               { return IsMouseButtonPressed(button); });
        }
    }
    else if (command == "WAIT_FOR_TAP")
    {
        co_await WaitUntil([this]()
                           { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                                    IsKeyPressed(KEY_SPACE) ||
                                    IsKeyPressed(KEY_ENTER); });
    }

    co_return;
}