// 【message_window】 メッセージ本文の表示、文字送り、ウィンドウフェードを管理する。
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

// クラス宣言: 公開操作と内部リソースをまとめる。
class MessageWindow
{
public:
    MessageWindow();
// メッセージ本文を一文字ずつ表示し、表示完了までコルーチンを待機させる。
    Task Message_Text_Type(const std::string &msg);
// 表示状態に応じてフェードイン・フェードアウトを行う。引数showで表示可否を指定する。
    Task ToggleShow(CoroutineManager &manager, bool show);
// 文字送り中の残りを即座に全表示する。
    void ShowAllText();
// 本文と表示済み文字数をリセットする。
    void ClearMessage();
// 現在の表示状態と透明度に応じて内容を描画する。
    void Graphic(Font font);

private:
// 表示対象の全文メッセージを保持する。
    std::string message;
// 文字送りで現在まで表示したUTF-8文字数を保持する。
    int displayedCharacters;
// 文字送りの1文字あたりの待機秒数を保持する。
    float textSpeed;
// 表示中かどうかを示し、描画・フェードの分岐に使う。
    bool is_visible = true;
// フェード処理の現在透明度を保持する。
    float windowAlpha = 0.0f;
};