#pragma once

#include <functional>
#include <utility>
#include <coroutine>

class WaitUntil
{
public:
    explicit WaitUntil(std::function<bool()> condition);

    bool await_ready() const;

    template <typename Promise>
    void await_suspend(std::coroutine_handle<Promise> handle)
    {
        auto &promise = handle.promise();

        promise.waiting = true;
        promise.remainingTime = 0.0f;
        promise.waitingForTask.reset();
        promise.waitCondition = std::move(condition_);
    }

    void await_resume() const;

private:
    std::function<bool()> condition_;
};