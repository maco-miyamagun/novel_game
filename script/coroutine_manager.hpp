#pragma once

#include "coroutine_task.hpp"

#include <vector>
#include <cstddef>

class CoroutineManager
{
public:
    CoroutineManager() = default;

    CoroutineManager(const CoroutineManager &) = delete;
    CoroutineManager &operator=(const CoroutineManager &) = delete;

    ~CoroutineManager();

    TaskCompletion Start(Task &&task);

    void Update();

    size_t GetTaskCount() const;

private:
    std::vector<Task::Handle> tasks_;
    std::vector<Task::Handle> pendingTasks_;

    bool updating_ = false;
};