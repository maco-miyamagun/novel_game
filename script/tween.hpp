// 【tween】 指定時間で数値を補間するコルーチンを提供する。
#pragma once

#include <functional>

#include "coroutine_task.hpp"
#include "coroutine_manager.hpp"

// 型宣言: コルーチンや描画処理で使う関連値をまとめる。
enum class EaseType
{
    LINEAR,
    EASEIN,
    EASEOUT,
    EASEINOUT
};

// 開始値から終了値へ補間し、各更新でcallbackへ現在値を渡す。
Task Tween(
    CoroutineManager &manager,
    float from,
    float to,
    float duration,
    EaseType easing,
    std::function<void(float)> callback);