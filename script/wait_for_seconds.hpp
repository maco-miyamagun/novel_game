// 【wait_for_seconds】 指定秒数経過までコルーチンを待機させる。
#pragma once

#include <coroutine>

// クラス宣言: 関連する状態、公開操作、内部リソースをまとめる。
class WaitForSeconds
{
public:
    explicit WaitForSeconds(float seconds);

// 待機不要なら即時再開可能かを返す。
    bool await_ready() const noexcept;

    template <typename Promise>
// コルーチンを待機状態へ移し、再開条件と残り時間を記録する。
    void await_suspend(
        std::coroutine_handle<Promise> handle) const noexcept
    {
        auto &promise = handle.promise();

// 主要メンバ変数: 時間以外の再開条件を保持する。
        promise.waitCondition = nullptr;
// 主要メンバ変数: コルーチンが待機状態かを示す。
        promise.waiting = true;
// 主要メンバ変数: 残り待機秒数を保持する。
        promise.remainingTime = seconds_;
// 主要メンバ変数: コルーチンが待機状態かを示す。
        promise.waitingForTask.reset();
    }

// 待機完了後に再開する。値は返さない。
    void await_resume() const noexcept;

private:
// 主要メンバ変数: 待機する秒数を保持する。
    float seconds_;
};