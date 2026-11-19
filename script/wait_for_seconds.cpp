// 【wait_for_seconds】 指定秒数経過までコルーチンの再開を待機する。
#include "wait_for_seconds.hpp"

WaitForSeconds::WaitForSeconds(float seconds)
    : seconds_(seconds)
{
}

// 待機不要なら即時再開可能かを返す。
bool WaitForSeconds::await_ready() const noexcept
{
    return seconds_ <= 0.0f;
}

// 待機完了後にコルーチンを再開する。値は返さない。
void WaitForSeconds::await_resume() const noexcept
{
}