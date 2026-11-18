
cmake_minimum_required(VERSION 3.20)

project(NovelGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==========================================
# OpenSSL
# ==========================================

find_package(OpenSSL REQUIRED COMPONENTS Crypto)

# ==========================================
# Game
# ==========================================

add_executable(NovelGame MACOSX_BUNDLE
    main.cpp
    external/stb/stb_image_impl.cpp
)

add_subdirectory(script)
add_subdirectory(game_program)

target_include_directories(NovelGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/script
    ${CMAKE_CURRENT_SOURCE_DIR}/game_program
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
)

# raylib
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

target_link_libraries(NovelGame PRIVATE
    /opt/homebrew/lib/libraylib.dylib
)

# ==========================================
# Bundle settings
# ==========================================

set_target_properties(NovelGame PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "NovelGame"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourname.novelgame"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
)

# ==========================================
# Asset Encryptor
# ==========================================

#暗号ライブラリ
add_executable(AssetEncryptorCLI
    tools/asset_encryptor_cli.cpp
)

target_link_libraries(AssetEncryptorCLI PRIVATE
    OpenSSL::Crypto
)

# 復号ライブラリ
add_library(AssetEncryptor STATIC
    tools/asset_encryptor.cpp
)

target_link_libraries(AssetEncryptor PUBLIC
    OpenSSL::Crypto
)

target_include_directories(AssetEncryptor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tools
)

# ゲーム本体にリンク
target_link_libraries(NovelGame PRIVATE
    AssetEncryptor
)

# ==========================================
# Resource discovery
# ==========================================

file(GLOB_RECURSE GAME_RESOURCE_FILES
    CONFIGURE_DEPENDS
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/*"
)

set(ENCRYPTED_ASSETS_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/encrypted_assets"
)

set(ENCRYPTED_ASSETS "")

# ==========================================
# Encrypt each resource
# ==========================================

foreach(RESOURCE_FILE IN LISTS GAME_RESOURCE_FILES)

    # resources/ からの相対パス
    file(RELATIVE_PATH RESOURCE_RELATIVE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "${RESOURCE_FILE}"
    )

    # 出力先
    set(ENCRYPTED_FILE
        "${ENCRYPTED_ASSETS_DIR}/${RESOURCE_RELATIVE_PATH}.enc"
    )

    # 出力先ディレクトリ
    get_filename_component(ENCRYPTED_DIR
        "${ENCRYPTED_FILE}"
        DIRECTORY
    )

    add_custom_command(
        OUTPUT "${ENCRYPTED_FILE}"

        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${ENCRYPTED_DIR}"

        COMMAND $<TARGET_FILE:AssetEncryptorCLI>
            e
            "${RESOURCE_FILE}"
            "${ENCRYPTED_FILE}"

        DEPENDS
            "${RESOURCE_FILE}"
            AssetEncryptor

        COMMENT "Encrypting: ${RESOURCE_RELATIVE_PATH}"

        VERBATIM
    )

    list(APPEND ENCRYPTED_ASSETS "${ENCRYPTED_FILE}")

endforeach()

# ==========================================
# Encrypted asset target
# ==========================================

add_custom_target(EncryptedAssets
    DEPENDS ${ENCRYPTED_ASSETS}
)

# ==========================================
# Deploy encrypted assets to .app
# ==========================================

add_custom_target(DeployEncryptedAssets ALL

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ENCRYPTED_ASSETS_DIR}"
        "$<TARGET_BUNDLE_CONTENT_DIR:NovelGame>/Resources"

    DEPENDS
        NovelGame
        EncryptedAssets

    COMMENT "Deploying encrypted assets to NovelGame.app"

    VERBATIM
)
