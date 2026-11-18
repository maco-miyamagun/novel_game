#include "wait_for_seconds.hpp"

WaitForSeconds::WaitForSeconds(float seconds)
    : seconds_(seconds)
{
}

bool WaitForSeconds::await_ready() const noexcept
{
    return seconds_ <= 0.0f;
}

void WaitForSeconds::await_resume() const noexcept
{
}