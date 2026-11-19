// 【wait_until】 条件成立までコルーチンを待機させる。
#pragma once

#include <functional>
#include <utility>
#include <coroutine>

// クラス宣言: 関連する状態、公開操作、内部リソースをまとめる。
class WaitUntil
{
public:
    explicit WaitUntil(std::function<bool()> condition);

// 待機不要なら即時再開可能かを返す。
    bool await_ready() const;

    template <typename Promise>
// コルーチンを待機状態へ移し、再開条件と残り時間を記録する。
    void await_suspend(std::coroutine_handle<Promise> handle)
    {
        auto &promise = handle.promise();

// 主要メンバ変数: コルーチンが待機状態かを示す。
        promise.waiting = true;
// 主要メンバ変数: 残り待機秒数を保持する。
        promise.remainingTime = 0.0f;
// 主要メンバ変数: コルーチンが待機状態かを示す。
        promise.waitingForTask.reset();
// 主要メンバ変数: 時間以外の再開条件を保持する。
        promise.waitCondition = std::move(condition_);
    }

// 待機完了後に再開する。値は返さない。
    void await_resume() const;

private:
// 主要メンバ変数: 成立後にコルーチンを再開する条件関数を保持する。
    std::function<bool()> condition_;
};