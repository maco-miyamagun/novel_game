// 【asset_manage】 暗号化資産の復号とraylibリソース生成を行う。
#pragma once
#include <string>
#include <raylib.h>
#include <unistd.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include "asset_encryptor.hpp"
#include <filesystem>
#include <utility>
#include "stb_image.h"
#include <climits>

// 実行ファイル位置を基準にリソースディレクトリへ移動する。
void ChangeToResourcesDirectory(void);
// 暗号化画像を復号してTexture2Dを生成する。失敗時は無効値を返す。
Texture2D LoadTextureFromFileEncrypted(const std::string &filename);
// 暗号化シナリオを復号した文字列バッファを返す。呼び出し側で解放する。
char *LoadScenarioFromFileEncrypted(const std::string &filename);
// 暗号化効果音を復号してSoundを生成する。
Sound LoadSEFromFileEncrypted(const std::string &filename);
// 暗号化BGMを復号し、Music本体と解放用バッファをペアで返す。
std::pair<Music, unsigned char *> LoadMusicFromFileEncrypted(const std::string &filename);
// 暗号化フォントを復号して指定コードポイントのFontを生成する。
Font LoadFontFromFileEncrypted(const std::string &filename, int fontSize, const int *codepoints, int codepointCount);
// 復号時に確保したバイト列を解放する。nullptrなら何もしない。
void UnloadEncryptedFileData(unsigned char *fileData);