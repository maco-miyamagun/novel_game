// 【coroutine_task】 コルーチンハンドルと完了状態の所有権を表す。
#pragma once

#include <coroutine>
#include <memory>
#include <functional>

// 型宣言: コルーチンや描画処理で使う関連値をまとめる。
struct CoroutineState
{
// 主要メンバ変数: タスクが正常完了したかを示し、待機先の再開判定に使う。
    bool completed = false;
};

// クラス宣言: 関連する状態、公開操作、内部リソースをまとめる。
class TaskCompletion
{
public:
    explicit TaskCompletion(std::shared_ptr<CoroutineState> state);

// 待機不要なら即時再開可能かを返す。
    bool await_ready() const noexcept;

    template <typename Promise>
// コルーチンを待機状態へ移し、再開条件と残り時間を記録する。
    void await_suspend(std::coroutine_handle<Promise> handle) const noexcept
    {
        auto &promise = handle.promise();

// 主要メンバ変数: コルーチンが待機状態かを示す。
        promise.waiting = true;
// 主要メンバ変数: 残り待機秒数を保持する。
        promise.remainingTime = 0.0f;
// 主要メンバ変数: コルーチンが待機状態かを示す。
        promise.waitingForTask = state_;
    }

// 待機完了後に再開する。値は返さない。
    void await_resume() const noexcept {}

private:
// 主要メンバ変数: TaskCompletionとPromise間で共有する完了状態を保持する。
    std::shared_ptr<CoroutineState> state_;
};

// クラス宣言: 関連する状態、公開操作、内部リソースをまとめる。
class Task
{
public:
// 型宣言: コルーチンや描画処理で使う関連値をまとめる。
    struct promise_type
    {
// 主要メンバ変数: コルーチンが待機状態かを示す。
        bool waiting = false;
// 主要メンバ変数: 残り待機秒数を保持する。
        float remainingTime = 0.0f;

        std::shared_ptr<CoroutineState> completion =
            std::make_shared<CoroutineState>();

// 主要メンバ変数: コルーチンが待機状態かを示す。
        std::shared_ptr<CoroutineState> waitingForTask;
// 主要メンバ変数: 時間以外の再開条件を保持する。
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

// コルーチンハンドルの所有権を切り離して返す。
    Handle Release();

// タスクの完了状態を共有するTaskCompletionを返す。
    TaskCompletion GetCompletion() const;

private:
// 主要メンバ変数: コルーチン本体の所有ハンドルを保持する。
    Handle handle_ = nullptr;
};