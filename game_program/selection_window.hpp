// 【selection_window】 選択肢のマウス操作、選択状態、表示切替を管理する。
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

// クラス宣言: 公開操作と内部リソースをまとめる。
class SelectionWindow
{
public:
    SelectionWindow();
// 選択肢を表示し、選択またはキャンセルまで待機する。
    Task ShowSelection(CoroutineManager &manager, const std::string &title, const std::vector<std::string> &options);
// マウス位置が選択肢矩形内ならホバー状態を更新する。
    void OnMouseHover(const Vector2 &mousePosition, Rectangle optionRect, int optionIndex);
// マウス押下と矩形内判定からクリック状態を更新する。
    void OnMouseClick(const Vector2 &mousePosition, Rectangle optionRect, int optionIndex);
// 経過時間とマウス操作から選択状態を更新する。引数deltaTimeは前フレームからの秒数。
    void Logic(float deltaTime);
// 表示状態に応じてフェードイン・フェードアウトを行う。引数showで表示可否を指定する。
    Task ToggleShow(CoroutineManager &manager, bool show);
// 現在の表示状態と透明度に応じて内容を描画する。
    void Graphic(Font font);
// 選択された選択肢のインデックスを返す。
    int GetSelectedOptionIndex();
// タイトル画面用の選択肢レイアウトを使用するか切り替える。
    void SetIsTitle(bool isTitle) { is_title = isTitle; }

private:
    Rectangle GetOptionRect(size_t optionIndex) const;

// 選択ウィンドウの見出しを保持する。
    std::string title;
// 表示する選択肢文字列の一覧を保持する。
    std::vector<std::string> options;
// 表示中かどうかを示し、描画・フェードの分岐に使う。
    bool is_visible = false;
// マウスが重なっている選択肢の位置を保持する。
    int hoveredOptionIndex = -1;
// クリックされた選択肢の位置を保持する。
    int clickedOptionIndex = -1;
// 確定選択された選択肢の位置を保持する。
    int selectedOptionIndex = 0;
// フェード処理の現在透明度を保持する。
    float windowAlpha = 0.0f;
// タイトル画面向けレイアウトを使うかを示す。
    bool is_title = false;
};
