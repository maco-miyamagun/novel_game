#include "coroutine_task.hpp"

#include <exception>
#include <utility>

TaskCompletion::TaskCompletion(
    std::shared_ptr<CoroutineState> state)
    : state_(std::move(state))
{
}

bool TaskCompletion::await_ready() const noexcept
{
    return !state_ || state_->completed;
}

Task Task::promise_type::get_return_object()
{
    return Task{
        std::coroutine_handle<promise_type>::from_promise(*this)};
}

std::suspend_always Task::promise_type::initial_suspend() noexcept
{
    return {};
}

std::suspend_always Task::promise_type::final_suspend() noexcept
{
    return {};
}

void Task::promise_type::return_void() noexcept
{
}

void Task::promise_type::unhandled_exception()
{
    std::terminate();
}

Task::Task(Handle handle)
    : handle_(handle)
{
}

Task::Task(Task &&other) noexcept
    : handle_(std::exchange(other.handle_, nullptr))
{
}

Task &Task::operator=(Task &&other) noexcept
{
    if (this != &other)
    {
        if (handle_)
            handle_.destroy();

        handle_ = std::exchange(other.handle_, nullptr);
    }

    return *this;
}

Task::~Task()
{
    if (handle_)
        handle_.destroy();
}

Task::Handle Task::Release()
{
    return std::exchange(handle_, nullptr);
}

TaskCompletion Task::GetCompletion() const
{
    if (!handle_)
        return TaskCompletion{nullptr};

    return TaskCompletion{handle_.promise().completion};
}