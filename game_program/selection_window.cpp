// 【selection_window】 選択肢のマウス操作、選択状態、表示切替を管理する。
#include "selection_window.hpp"

SelectionWindow::SelectionWindow()
{
    title = "";
    options.clear();
    is_visible = false;
    hoveredOptionIndex = -1;
    clickedOptionIndex = -1;
    selectedOptionIndex = 0;
    windowAlpha = 0.0f;
}

// 選択肢を表示し、選択またはキャンセルまでコルーチンを待機させる。
Task SelectionWindow::ShowSelection(CoroutineManager &manager, const std::string &title, const std::vector<std::string> &options)
{
    this->title = title;
    this->options = options;

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
    co_await manager.Start(ToggleShow(manager, true));

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
    co_await WaitUntil([this]()
                       { return clickedOptionIndex != -1; });

    selectedOptionIndex = clickedOptionIndex;
    clickedOptionIndex = -1; // クリック状態をリセット
    hoveredOptionIndex = -1; // ホバー状態をリセット

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
    co_await manager.Start(ToggleShow(manager, false));
// コルーチン終了: 現在の処理分岐から正常に抜ける。
    co_return;
}

// マウス位置が選択肢矩形内ならホバー状態を更新する。
void SelectionWindow::OnMouseHover(const Vector2 &mousePosition, Rectangle optionRect, int optionIndex)
{
    if (CheckCollisionPointRec(mousePosition, optionRect))
    {
        hoveredOptionIndex = optionIndex;
    }
    else if (hoveredOptionIndex == optionIndex)
    {
        hoveredOptionIndex = -1;
    }
}

// マウス押下と矩形内判定からクリック状態を更新する。
void SelectionWindow::OnMouseClick(const Vector2 &mousePosition, Rectangle optionRect, int optionIndex)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, optionRect))
    {
        clickedOptionIndex = optionIndex;
    }
    else if (clickedOptionIndex == optionIndex)
    {
        clickedOptionIndex = -1;
    }
}

// 経過時間とマウス操作から選択状態を更新する。引数deltaTimeは前フレームからの秒数。
void SelectionWindow::Logic(float deltaTime)
{
    if (!is_visible)
        return;

// ウィンドウ座標のマウス位置をゲーム座標へ変換して返す。
    Vector2 mousePosition = GetVirtualMousePosition();

// ループ: 対象を順番に処理し、各要素の状態を確認する。
    for (size_t i = 0; i < options.size(); ++i)
    {
        Rectangle optionRect = GetOptionRect(i);

        OnMouseHover(mousePosition, optionRect, static_cast<int>(i));
        OnMouseClick(mousePosition, optionRect, static_cast<int>(i));
    }
}

// 選択肢インデックスから表示矩形を計算する。
Rectangle SelectionWindow::GetOptionRect(size_t optionIndex) const
{
    const float centerX = GAME_WIDTH / 2.0f + SelectionWindowLayout::X;
    const float centerY = GAME_HEIGHT / 2.0f -
                          (SelectionWindowLayout::FIRST_OPTION_Y -
                           static_cast<float>(optionIndex) *
                               (SelectionWindowLayout::OPTION_HEIGHT +
                                SelectionWindowLayout::OPTION_GAP));

    return {
        centerX - SelectionWindowLayout::OPTION_WIDTH / 2.0f,
        centerY - SelectionWindowLayout::OPTION_HEIGHT / 2.0f,
        SelectionWindowLayout::OPTION_WIDTH,
        SelectionWindowLayout::OPTION_HEIGHT};
}

// 表示状態に応じてフェードイン・フェードアウトを行う。引数showがtrueなら表示、falseなら非表示にする。
Task SelectionWindow::ToggleShow(CoroutineManager &manager, bool show)
{
    if (show)
    {
        is_visible = true;

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

// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }

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

    is_visible = false;
}

// 現在の表示状態と透明度に応じて内容を描画する。
void SelectionWindow::Graphic(Font font)
{
    if (!is_visible)
        return;

    float base_scale = windowAlpha;

    if (!is_title)
    {
        Graphic_Rectangle(
            SelectionWindowLayout::X * base_scale,
            SelectionWindowLayout::Y * base_scale,
            SelectionWindowLayout::WIDTH * base_scale,
            SelectionWindowLayout::HEIGHT * base_scale,
            Fade(BLACK, 0.86f * windowAlpha));

        Graphic_RectangleLines(
            SelectionWindowLayout::X * base_scale,
            SelectionWindowLayout::Y * base_scale,
            SelectionWindowLayout::WIDTH * base_scale,
            SelectionWindowLayout::HEIGHT * base_scale,
            7.0f,
            Fade(WHITE, windowAlpha));
    }

    Graphic_Text(
        font,
        title.c_str(),
        SelectionWindowLayout::X * base_scale,
        SelectionWindowLayout::TITLE_Y * base_scale,
        SelectionWindowLayout::TITLE_FONT_SIZE * base_scale,
        SelectionWindowLayout::OPTION_TEXT_SPACING,
        Fade(WHITE, windowAlpha),
        true, true);

// ループ: 対象を順番に処理し、各要素の状態を確認する。
    for (size_t i = 0; i < options.size(); ++i)
    {
        const float optionY =
            SelectionWindowLayout::FIRST_OPTION_Y -
            static_cast<float>(i) *
                (SelectionWindowLayout::OPTION_HEIGHT +
                 SelectionWindowLayout::OPTION_GAP);
        const Rectangle optionRect = GetOptionRect(i);

        const Color optionColor =
            hoveredOptionIndex == static_cast<int>(i)
                ? Color{200, 200, 200, static_cast<unsigned char>(windowAlpha * 255)}
                : Color{150, 150, 150, static_cast<unsigned char>(windowAlpha * 255)};

        Graphic_Rectangle(
            SelectionWindowLayout::X * base_scale,
            optionY * base_scale,
            SelectionWindowLayout::OPTION_WIDTH * base_scale,
            SelectionWindowLayout::OPTION_HEIGHT * base_scale,
            optionColor);

        Graphic_Text(
            font,
            options[i].c_str(),
            SelectionWindowLayout::X * base_scale,
            optionY * base_scale,
            SelectionWindowLayout::OPTION_FONT_SIZE * base_scale,
            SelectionWindowLayout::OPTION_TEXT_SPACING,
            Fade(BLACK, windowAlpha),
            true, true);
    }
}

// 選択された選択肢のインデックスを返す。
int SelectionWindow::GetSelectedOptionIndex()
{
    return selectedOptionIndex;
}