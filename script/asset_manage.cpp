#include "asset_manage.hpp"

const char *envPassword = "faihfoiahghasgaioghoahohgohirag";

void ChangeToResourcesDirectory(void)
{
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0)
    {
        // 実行ファイルがあるディレクトリを取得 (YourGame.app/Contents/MacOS)
        char *dir = dirname(path);
        chdir(dir);
        // Resources ディレクトリへ移動
        chdir("../Resources");
    }
}

Texture2D LoadTextureFromFileEncrypted(const std::string &filename)
{
    std::string extension =
        std::filesystem::path(filename).extension().string();

    unsigned char *fileData = nullptr;
    size_t fileSize = 0;

    // 復号
    if (!AssetEncryptor::DecryptToMemory(
            filename + ".enc",
            envPassword,
            &fileData,
            &fileSize))
    {
        TraceLog(LOG_ERROR, "Failed to decrypt image file: %s",
                 filename.c_str());
        return {};
    }

    if (fileSize > static_cast<size_t>(INT_MAX))
    {
        AssetEncryptor::Free(fileData);
        return {};
    }

    int width = 0;
    int height = 0;
    int channels = 0;

    // stb_imageでメモリから画像を読み込む
    unsigned char *pixels = stbi_load_from_memory(
        fileData,
        static_cast<int>(fileSize),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);

    // 復号データの解放
    AssetEncryptor::Free(fileData);

    if (pixels == nullptr)
    {
        TraceLog(LOG_ERROR, "Failed to load image: %s (%s)",
                 filename.c_str(),
                 stbi_failure_reason());
        return {};
    }

    // raylib Imageを構築
    Image image = {};

    image.data = pixels;
    image.width = width;
    image.height = height;
    image.mipmaps = 1;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    // GPUテクスチャに変換
    Texture2D texture = LoadTextureFromImage(image);

    // stb_imageで確保したメモリを解放
    stbi_image_free(pixels);

    if (texture.id == 0)
        return {};

    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);

    return texture;
}

// Texture2D LoadTextureFromFileEncrypted(const std::string &filename)
// {
//     std::string extension =
//         std::filesystem::path(filename).extension().string();

//     unsigned char *fileData = nullptr;
//     size_t fileSize = 0;

//     if (!AssetEncryptor::DecryptToMemory(
//             filename + ".enc",
//             envPassword,
//             &fileData,
//             &fileSize))
//     {
//         TraceLog(LOG_ERROR, "Failed to decrypt scenario file: %s", filename.c_str());
//         return {};
//     }

//     if (fileSize > static_cast<size_t>(INT_MAX))
//     {
//         AssetEncryptor::Free(fileData);
//         return {};
//     }

//     Image image = LoadImageFromMemory(
//         extension.c_str(),
//         fileData,
//         static_cast<int>(fileSize));

//     AssetEncryptor::Free(fileData);

//     if (image.data == nullptr)
//         return {};

//     Texture2D texture = LoadTextureFromImage(image);

//     UnloadImage(image);

//     SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
//     return texture;
// }

char *LoadScenarioFromFileEncrypted(const std::string &filename)
{
    unsigned char *fileData = nullptr;
    size_t fileSize = 0;

    if (!AssetEncryptor::DecryptToMemory(
            filename + ".enc",
            envPassword,
            &fileData,
            &fileSize))
    {
        TraceLog(LOG_ERROR, "Failed to decrypt scenario file: %s", filename.c_str());
        return {};
    }

    if (fileSize > static_cast<size_t>(INT_MAX))
    {
        AssetEncryptor::Free(fileData);
        return {};
    }

    char *text_data = new char[fileSize + 1];
    std::memcpy(text_data, fileData, fileSize);
    text_data[fileSize] = '\0';

    AssetEncryptor::Free(fileData);

    return text_data;
}

Sound LoadSEFromFileEncrypted(const std::string &filename)
{
    std::string extension =
        std::filesystem::path(filename).extension().string();

    unsigned char *fileData = nullptr;
    size_t fileSize = 0;

    if (!AssetEncryptor::DecryptToMemory(
            filename + ".enc",
            envPassword,
            &fileData,
            &fileSize))
    {
        return {};
    }

    if (fileSize > static_cast<size_t>(INT_MAX))
    {
        AssetEncryptor::Free(fileData);
        return {};
    }

    Wave wave = LoadWaveFromMemory(
        extension.c_str(),
        fileData,
        static_cast<int>(fileSize));

    AssetEncryptor::Free(fileData);

    // WaveからSoundを生成
    Sound sound = LoadSoundFromWave(wave);

    UnloadWave(wave);

    if (sound.frameCount == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load sound effect: %s", filename.c_str());
        return {};
    }

    return sound;
}

std::pair<Music, unsigned char *> LoadMusicFromFileEncrypted(const std::string &filename)
{
    std::string extension =
        std::filesystem::path(filename).extension().string();

    unsigned char *fileData = nullptr;
    size_t fileSize = 0;

    if (!AssetEncryptor::DecryptToMemory(
            filename + ".enc",
            envPassword,
            &fileData,
            &fileSize))
    {
        return {};
    }

    if (fileSize > static_cast<size_t>(INT_MAX))
    {
        AssetEncryptor::Free(fileData);
        return {};
    }

    Music music = LoadMusicStreamFromMemory(
        extension.c_str(),
        fileData,
        static_cast<int>(fileSize));

    if (music.frameCount == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load music: %s", filename.c_str());
        return {};
    }

    return {music, fileData};
}

Font LoadFontFromFileEncrypted(const std::string &filename, int fontSize, const int *codepoints, int codepointCount)
{
    std::string extension =
        std::filesystem::path(filename).extension().string();

    unsigned char *fileData = nullptr;
    size_t fileSize = 0;

    if (!AssetEncryptor::DecryptToMemory(
            filename + ".enc",
            envPassword,
            &fileData,
            &fileSize))
    {
        return {};
    }

    if (fileSize > static_cast<size_t>(INT_MAX))
    {
        AssetEncryptor::Free(fileData);
        return {};
    }

    Font font = LoadFontFromMemory(
        extension.c_str(),
        fileData,
        static_cast<int>(fileSize),
        fontSize,
        codepoints,
        codepointCount);

    AssetEncryptor::Free(fileData);

    if (font.texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load font: %s", filename.c_str());
        return {};
    }

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    return font;
}

void UnloadEncryptedFileData(unsigned char *fileData)
{
    AssetEncryptor::Free(fileData);
}