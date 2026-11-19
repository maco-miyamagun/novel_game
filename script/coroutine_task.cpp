// 【coroutine_task】 コルーチンの所有権、完了状態、子タスク待機を表す。
#include "coroutine_task.hpp"

#include <exception>
#include <utility>

TaskCompletion::TaskCompletion(
    std::shared_ptr<CoroutineState> state)
    : state_(std::move(state))
{
}

// 待機不要なら即時再開可能かを返す。
bool TaskCompletion::await_ready() const noexcept
{
    return !state_ || state_->completed;
}

// タスク本体のハンドルからTaskオブジェクトを生成する。
Task Task::promise_type::get_return_object()
{
    return Task{
        std::coroutine_handle<promise_type>::from_promise(*this)};
}

// タスク開始時に停止し、明示的なスケジューリングを可能にする。
std::suspend_always Task::promise_type::initial_suspend() noexcept
{
    return {};
}

// タスク終了時に停止し、完了状態を確定する。
std::suspend_always Task::promise_type::final_suspend() noexcept
{
    return {};
}

// 戻り値なしでコルーチンを正常完了する。
void Task::promise_type::return_void() noexcept
{
}

// 捕捉されなかった例外を未処理例外として保持または伝播する。
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

// コルーチンハンドルの所有権をタスクオブジェクトから切り離して返す。
Task::Handle Task::Release()
{
    return std::exchange(handle_, nullptr);
}

// このタスクの完了状態を共有するTaskCompletionを返す。
TaskCompletion Task::GetCompletion() const
{
    if (!handle_)
        return TaskCompletion{nullptr};

    return TaskCompletion{handle_.promise().completion};
}