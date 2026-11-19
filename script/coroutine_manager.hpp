// 【coroutine_manager】 コルーチンタスクの登録、待機、更新、終了を管理する。
#pragma once

#include "coroutine_task.hpp"

#include <vector>
#include <cstddef>

// クラス宣言: 関連する状態、公開操作、内部リソースをまとめる。
class CoroutineManager
{
public:
    CoroutineManager() = default;

    CoroutineManager(const CoroutineManager &) = delete;
    CoroutineManager &operator=(const CoroutineManager &) = delete;

    ~CoroutineManager();

// タスクを登録し、完了状態を識別するTaskCompletionを返す。
    TaskCompletion Start(Task &&task);

// 登録タスクの待機条件・残り時間・完了状態を更新する。
    void Update();

// 現在管理中のタスク数を返す。
    size_t GetTaskCount() const;

private:
// 主要メンバ変数: 管理中のコルーチンハンドル一覧を保持する。
    std::vector<Task::Handle> tasks_;
// 主要メンバ変数: Update中に追加されたタスクを次回処理へ回すため保持する。
    std::vector<Task::Handle> pendingTasks_;

// 主要メンバ変数: Updateの再入を防ぐため更新中かを示す。
    bool updating_ = false;
};