#pragma once

#include <functional>

#include "coroutine_task.hpp"
#include "coroutine_manager.hpp"

enum class EaseType
{
    LINEAR,
    EASEIN,
    EASEOUT,
    EASEINOUT
};

Task Tween(
    CoroutineManager &manager,
    float from,
    float to,
    float duration,
    EaseType easing,
    std::function<void(float)> callback);