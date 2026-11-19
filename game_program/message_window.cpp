// 【message_window】 メッセージ本文の表示、文字送り、ウィンドウフェードを管理する。
#include "message_window.hpp"

MessageWindow::MessageWindow()
    : displayedCharacters(0),
      textSpeed(0.035f)
{
}

// メッセージ本文を一文字ずつ表示し、表示完了までコルーチンを待機させる。
Task MessageWindow::Message_Text_Type(const std::string &msg)
{
    message = msg;
    displayedCharacters = 0;

// ループ: 終了条件を満たすまで繰り返して状態を更新する。
    while (displayedCharacters < UTF8Length(message))
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await WaitForSeconds(textSpeed);
        displayedCharacters++;

        // if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) ||
        //     IsKeyPressed(KEY_SPACE) ||
        //     IsKeyPressed(KEY_ENTER))
        // {
        //     ShowAllText();
        //     co_await WaitForSeconds(0.05f);
        //     co_return;
        // }
    }

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
    co_await WaitUntil([this]()
                       { return IsMouseButtonDown(MOUSE_BUTTON_LEFT) ||
                                IsKeyPressed(KEY_SPACE) ||
                                IsKeyPressed(KEY_ENTER); });
}

// 表示状態に応じてフェードイン・フェードアウトを行う。引数showがtrueなら表示、falseなら非表示にする。
Task MessageWindow::ToggleShow(CoroutineManager &manager, bool show)
{
    ClearMessage();

    if (show)
    {
        is_visible = show;

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await manager.Start(Tween(
            manager,
            windowAlpha,
            1.0f,
            0.5f,
            EaseType::EASEIN,
            [this](float value)
            {
                windowAlpha = value;
            }));
    }
    else
    {
// コルーチン待機: 処理完了または条件成立まで再開を保留する。
        co_await manager.Start(Tween(
            manager,
            windowAlpha,
            0.0f,
            0.5f,
            EaseType::EASEOUT,
            [this](float value)
            {
                windowAlpha = value;
            }));

        is_visible = show;
    }
}

// 文字送り中の残りを即座に全表示する。
void MessageWindow::ShowAllText()
{
    displayedCharacters = UTF8Length(message);
}

// 本文と表示済み文字数をリセットする。
void MessageWindow::ClearMessage()
{
    message.clear();
    displayedCharacters = 0;
}

// 現在の表示状態と透明度に応じて内容を描画する。
void MessageWindow::Graphic(Font font)
{
    if (!is_visible)
        return;

    float base_scale = windowAlpha;

    // Draw the message window background
    Graphic_Rectangle(
        0, -500,
        2400 * base_scale, 340 * base_scale,
        Fade(BLACK, 0.82f * windowAlpha));

    Graphic_RectangleLines(
        0, -500,
        2400 * base_scale, 340 * base_scale,
        7,
        Fade(WHITE, windowAlpha));

    // Draw the message text
    std::string displayedMessage = GetUTF8Substring(message, 0, displayedCharacters);

    Graphic_Text(
        font,
        displayedMessage.c_str(),
        -1140 * base_scale,
        -410 * base_scale,
        64 * base_scale,
        4,
        Fade(WHITE, windowAlpha),
        false, false);

    if (!(displayedCharacters < UTF8Length(message)) && UTF8Length(message) != 0)
    {
        Graphic_Text(
            font,
            "Next",
            1070 * base_scale,
            -580 * base_scale,
            64 * base_scale,
            2,
            Fade(WHITE, windowAlpha),
            true, true);
    }
}