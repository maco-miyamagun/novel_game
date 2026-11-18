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

class BackgroundManager
{
public:
    BackgroundManager();
    Task ToggleShow(CoroutineManager &manager, bool show);
    void QuickChangeBackground(const std::string &filePath, const std::string &scene_name = "");
    int LoadAndPoolTexture(const std::string &filePath);
    int IsExistingTexture(const std::string &filePath) const;
    void Graphic(Font font);
    void UnloadAllBackgroundTextures();

private:
    std::vector<Texture2D> texture_pool;
    std::vector<std::string> texture_paths;
    int current_texture_index = 0;
    bool is_visible = true;
    float backgroundAlpha = 0.0f;
    std::string current_scene_name = "";
};