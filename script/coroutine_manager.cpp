#include "coroutine_manager.hpp"

#include <raylib.h>
#include <utility>

CoroutineManager::~CoroutineManager()
{
    for (auto handle : tasks_)
    {
        if (handle)
            handle.destroy();
    }

    for (auto handle : pendingTasks_)
    {
        if (handle)
            handle.destroy();
    }

    tasks_.clear();
    pendingTasks_.clear();
}

TaskCompletion CoroutineManager::Start(Task &&task)
{
    auto completion = task.GetCompletion();
    auto handle = task.Release();

    if (handle)
    {
        if (updating_)
            pendingTasks_.push_back(handle);
        else
            tasks_.push_back(handle);
    }

    return completion;
}

void CoroutineManager::Update()
{
    updating_ = true;

    const float deltaTime = GetFrameTime();

    auto it = tasks_.begin();

    while (it != tasks_.end())
    {
        auto handle = *it;
        auto &promise = handle.promise();

        if (promise.waiting)
        {
            // 別コルーチンの完了待ち
            if (promise.waitingForTask)
            {
                if (!promise.waitingForTask->completed)
                {
                    ++it;
                    continue;
                }

                promise.waitingForTask.reset();
            }
            // 条件成立待ち
            else if (promise.waitCondition)
            {
                if (!promise.waitCondition())
                {
                    ++it;
                    continue;
                }

                promise.waitCondition = nullptr;
            }
            // 時間待ち
            else
            {
                promise.remainingTime -= deltaTime;

                if (promise.remainingTime > 0.0f)
                {
                    ++it;
                    continue;
                }
            }

            promise.waiting = false;
        }

        handle.resume();

        if (handle.done())
        {
            promise.completion->completed = true;

            handle.destroy();

            it = tasks_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    updating_ = false;

    // 更新中に登録された Coroutine を追加
    for (auto handle : pendingTasks_)
    {
        tasks_.push_back(handle);
    }

    pendingTasks_.clear();
}

size_t CoroutineManager::GetTaskCount() const
{
    return tasks_.size() + pendingTasks_.size();
}