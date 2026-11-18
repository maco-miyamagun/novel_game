#pragma once

#include <coroutine>
#include <memory>
#include <functional>

struct CoroutineState
{
    bool completed = false;
};

class TaskCompletion
{
public:
    explicit TaskCompletion(std::shared_ptr<CoroutineState> state);

    bool await_ready() const noexcept;

    template <typename Promise>
    void await_suspend(std::coroutine_handle<Promise> handle) const noexcept
    {
        auto &promise = handle.promise();

        promise.waiting = true;
        promise.remainingTime = 0.0f;
        promise.waitingForTask = state_;
    }

    void await_resume() const noexcept {}

private:
    std::shared_ptr<CoroutineState> state_;
};

class Task
{
public:
    struct promise_type
    {
        bool waiting = false;
        float remainingTime = 0.0f;

        std::shared_ptr<CoroutineState> completion =
            std::make_shared<CoroutineState>();

        std::shared_ptr<CoroutineState> waitingForTask;
        std::function<bool()> waitCondition;

        Task get_return_object();

        std::suspend_always initial_suspend() noexcept;
        std::suspend_always final_suspend() noexcept;

        void return_void() noexcept;
        void unhandled_exception();
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Task(Handle handle);

    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;

    Task(Task &&other) noexcept;
    Task &operator=(Task &&other) noexcept;

    ~Task();

    Handle Release();

    TaskCompletion GetCompletion() const;

private:
    Handle handle_ = nullptr;
};