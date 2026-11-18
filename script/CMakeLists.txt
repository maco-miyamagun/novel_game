target_sources(NovelGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/coroutine_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coroutine_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utf8.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/game_settings.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/asset_manage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/japanese_font.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/game_screen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wait_for_seconds.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wait_until.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tween.cpp
)
