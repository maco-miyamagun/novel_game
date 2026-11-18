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

void ChangeToResourcesDirectory(void);
Texture2D LoadTextureFromFileEncrypted(const std::string &filename);
char *LoadScenarioFromFileEncrypted(const std::string &filename);
Sound LoadSEFromFileEncrypted(const std::string &filename);
std::pair<Music, unsigned char *> LoadMusicFromFileEncrypted(const std::string &filename);
Font LoadFontFromFileEncrypted(const std::string &filename, int fontSize, const int *codepoints, int codepointCount);
void UnloadEncryptedFileData(unsigned char *fileData);