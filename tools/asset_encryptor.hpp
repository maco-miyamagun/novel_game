
#pragma once

#include <cstddef>
#include <string>

namespace AssetEncryptor
{
    /// 暗号化ファイルをメモリ上に復号する
    ///
    /// @param inputPath 暗号化ファイルのパス
    /// @param password 復号パスワード
    /// @param fileData 復号データの格納先
    /// @param fileSize 復号データのサイズ
    ///
    /// @return 成功した場合 true
    bool DecryptToMemory(
        const std::string &inputPath,
        const std::string &password,
        unsigned char **fileData,
        size_t *fileSize);

    /// DecryptToMemoryで確保したメモリを解放する
    void Free(unsigned char *fileData);
}
