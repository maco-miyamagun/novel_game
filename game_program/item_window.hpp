#pragma once

#include "coroutine_task.hpp"
#include "coroutine_manager.hpp"
#include "wait_for_seconds.hpp"
#include "wait_until.hpp"
#include "tween.hpp"
#include "graphic.hpp"
#include "raylib.h"
#include <string>
#include <vector>
#include "asset_manage.hpp"
#include "game_settings.hpp"

class ItemWindow
{
public:
    ItemWindow();
    Task OpenWin(CoroutineManager &manager, const std::string &filePath);
    int LoadAndPoolTexture(const std::string &filePath);
    int IsExistingTexture(const std::string &filePath) const;
    void Graphic();
    void UnloadAllItemTextures();

private:
    std::vector<Texture2D> texture_pool;
    std::vector<std::string> texture_paths;
    int current_texture_index = 0;
    bool is_visible = true;
    float alpha = 0.0f;
};