#pragma once

#include <coroutine>

class WaitForSeconds
{
public:
    explicit WaitForSeconds(float seconds);

    bool await_ready() const noexcept;

    template <typename Promise>
    void await_suspend(
        std::coroutine_handle<Promise> handle) const noexcept
    {
        auto &promise = handle.promise();

        promise.waitCondition = nullptr;
        promise.waiting = true;
        promise.remainingTime = seconds_;
        promise.waitingForTask.reset();
    }

    void await_resume() const noexcept;

private:
    float seconds_;
};