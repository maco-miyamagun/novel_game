// 【tween】 指定時間で数値を補間し、各フレームでコールバックを呼ぶ。
#include "tween.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include "raylib.h"
#include "wait_until.hpp"

namespace
{
    float ApplyEasing(float t, EaseType easing)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        switch (easing)
        {
        case EaseType::LINEAR:
            return t;

        case EaseType::EASEIN:
            return t * t;

        case EaseType::EASEOUT:
            return 1.0f - (1.0f - t) * (1.0f - t);

        case EaseType::EASEINOUT:
            if (t < 0.5f)
                return 2.0f * t * t;

            return 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
        }

        return t;
    }
}

// 開始値から終了値へ補間し、各更新でcallbackへ現在値を渡す。
Task Tween(
    CoroutineManager &manager,
    float from,
    float to,
    float duration,
    EaseType easing,
    std::function<void(float)> callback)
{
    if (duration <= 0.0f)
    {
        if (callback)
            callback(to);

// コルーチン終了: 現在の処理分岐から正常に抜ける。
        co_return;
    }

    float elapsed = 0.0f;

    if (callback)
        callback(from);

// コルーチン待機: 処理完了または条件成立まで再開を保留する。
    co_await WaitUntil([&]()
                       {
        elapsed += GetFrameTime();

        float t = std::clamp(elapsed / duration, 0.0f, 1.0f);
        float eased = ApplyEasing(t, easing);

        float value = from + (to - from) * eased;

        if (callback)
            callback(value);

        return t >= 1.0f; });
}