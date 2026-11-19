// 【wait_until】 条件が真になるまでコルーチンの再開を待機する。
#include "wait_until.hpp"

WaitUntil::WaitUntil(std::function<bool()> condition)
    : condition_(std::move(condition))
{
}

// 待機不要なら即時再開可能かを返す。
bool WaitUntil::await_ready() const
{
    return condition_();
}

// 待機完了後にコルーチンを再開する。値は返さない。
void WaitUntil::await_resume() const
{
}