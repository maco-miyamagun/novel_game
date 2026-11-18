#include "wait_until.hpp"

WaitUntil::WaitUntil(std::function<bool()> condition)
    : condition_(std::move(condition))
{
}

bool WaitUntil::await_ready() const
{
    return condition_();
}

void WaitUntil::await_resume() const
{
}