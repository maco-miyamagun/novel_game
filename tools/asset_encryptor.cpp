#include "asset_encryptor.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <limits>
#include <memory>
#include <new>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

namespace AssetEncryptor
{
    namespace
    {
        constexpr int KEY_SIZE = 32;
        constexpr int SALT_SIZE = 16;
        constexpr int NONCE_SIZE = 12;
        constexpr int TAG_SIZE = 16;

        constexpr int PBKDF2_ITERATIONS = 600000;

        constexpr char MAGIC[] = "RAYENC01";

        constexpr size_t HEADER_SIZE =
            8 + SALT_SIZE + NONCE_SIZE;

        using Bytes = std::vector<unsigned char>;

        using CipherContext =
            std::unique_ptr<
                EVP_CIPHER_CTX,
                decltype(&EVP_CIPHER_CTX_free)>;

        // ======================================
        // File Read
        // ======================================

        bool ReadFile(
            const std::string &path,
            Bytes &data)
        {
            std::ifstream file(path, std::ios::binary);

            if (!file)
                return false;

            file.seekg(0, std::ios::end);

            const auto size = file.tellg();

            if (size < 0)
                return false;

            if (static_cast<uintmax_t>(size) >
                static_cast<uintmax_t>(
                    std::numeric_limits<size_t>::max()))
                return false;

            if (static_cast<uintmax_t>(size) >
                static_cast<uintmax_t>(
                    std::numeric_limits<std::streamsize>::max()))
                return false;

            file.seekg(0, std::ios::beg);

            data.resize(static_cast<size_t>(size));

            if (!data.empty())
            {
                file.read(
                    reinterpret_cast<char *>(data.data()),
                    static_cast<std::streamsize>(data.size()));

                if (!file)
                    return false;
            }

            return true;
        }

        // ======================================
        // Key Derivation
        // ======================================

        bool DeriveKey(
            const std::string &password,
            const unsigned char *salt,
            std::array<unsigned char, KEY_SIZE> &key)
        {
            if (password.empty() ||
                password.size() >
                    static_cast<size_t>(
                        std::numeric_limits<int>::max()))
            {
                return false;
            }

            return PKCS5_PBKDF2_HMAC(
                       password.data(),
                       static_cast<int>(password.size()),
                       salt,
                       SALT_SIZE,
                       PBKDF2_ITERATIONS,
                       EVP_sha256(),
                       KEY_SIZE,
                       key.data()) == 1;
        }
    }

    // ==========================================
    // Decrypt To Memory
    // ==========================================

    bool DecryptToMemory(
        const std::string &inputPath,
        const std::string &password,
        unsigned char **fileData,
        size_t *fileSize)
    {
        // Initialize outputs.

        if (fileData == nullptr || fileSize == nullptr)
            return false;

        *fileData = nullptr;
        *fileSize = 0;

        // Read encrypted file.

        Bytes input;

        if (!ReadFile(inputPath, input))
        {
            std::cerr << "Failed to read encrypted file.\n";
            return false;
        }

        if (input.size() < HEADER_SIZE + TAG_SIZE)
        {
            std::cerr << "Invalid encrypted file.\n";
            return false;
        }

        // Validate header.

        if (!std::equal(
                MAGIC,
                MAGIC + 8,
                input.begin()))
        {
            std::cerr << "Invalid file format.\n";
            return false;
        }

        // Extract encryption parameters.

        const unsigned char *salt =
            input.data() + 8;

        const unsigned char *nonce =
            input.data() + 8 + SALT_SIZE;

        const size_t cipherSize =
            input.size() - HEADER_SIZE - TAG_SIZE;

        const unsigned char *cipherText =
            input.data() + HEADER_SIZE;

        const unsigned char *tag =
            input.data() + HEADER_SIZE + cipherSize;

        // Derive key.

        std::array<unsigned char, KEY_SIZE> key{};

        if (!DeriveKey(password, salt, key))
        {
            std::cerr << "Key derivation failed.\n";
            return false;
        }

        // Initialize AES-GCM.

        CipherContext ctx(
            EVP_CIPHER_CTX_new(),
            EVP_CIPHER_CTX_free);

        if (!ctx)
            return false;

        if (EVP_DecryptInit_ex(
                ctx.get(),
                EVP_aes_256_gcm(),
                nullptr,
                nullptr,
                nullptr) != 1)
            return false;

        if (EVP_CIPHER_CTX_ctrl(
                ctx.get(),
                EVP_CTRL_GCM_SET_IVLEN,
                NONCE_SIZE,
                nullptr) != 1)
            return false;

        if (EVP_DecryptInit_ex(
                ctx.get(),
                nullptr,
                nullptr,
                key.data(),
                nonce) != 1)
            return false;

        // Authenticate header (AAD).

        int len = 0;

        if (EVP_DecryptUpdate(
                ctx.get(),
                nullptr,
                &len,
                input.data(),
                static_cast<int>(HEADER_SIZE)) != 1)
            return false;

        // Decrypt ciphertext.

        if (cipherSize >
            std::numeric_limits<size_t>::max() -
                EVP_MAX_BLOCK_LENGTH)
            return false;

        Bytes decrypted(
            cipherSize + EVP_MAX_BLOCK_LENGTH);

        size_t offset = 0;
        size_t written = 0;

        while (offset < cipherSize)
        {
            const int chunk = static_cast<int>(
                std::min(
                    cipherSize - offset,
                    static_cast<size_t>(
                        std::numeric_limits<int>::max())));

            if (EVP_DecryptUpdate(
                    ctx.get(),
                    decrypted.data() + written,
                    &len,
                    cipherText + offset,
                    chunk) != 1)
            {
                OPENSSL_cleanse(key.data(), key.size());
                return false;
            }

            offset += static_cast<size_t>(chunk);
            written += static_cast<size_t>(len);
        }

        // Set authentication tag.

        if (EVP_CIPHER_CTX_ctrl(
                ctx.get(),
                EVP_CTRL_GCM_SET_TAG,
                TAG_SIZE,
                const_cast<unsigned char *>(tag)) != 1)
        {
            OPENSSL_cleanse(key.data(), key.size());
            return false;
        }

        // Verify authentication.

        // Wrong password or modified file will fail here.

        if (EVP_DecryptFinal_ex(
                ctx.get(),
                decrypted.data() + written,
                &len) != 1)
        {
            OPENSSL_cleanse(key.data(), key.size());

            std::cerr
                << "Authentication failed.\n"
                << "Wrong password or corrupted file.\n";

            return false;
        }

        written += static_cast<size_t>(len);

        // Key is no longer needed.

        OPENSSL_cleanse(key.data(), key.size());

        // Allocate output memory only after authentication.

        std::unique_ptr<unsigned char[]> result(
            new (std::nothrow) unsigned char[std::max<size_t>(written, 1)]);

        if (!result)
            return false;

        if (written > 0)
        {
            std::copy_n(
                decrypted.data(),
                written,
                result.get());
        }

        // Transfer ownership to caller.

        *fileData = result.release();
        *fileSize = written;

        return true;
    }

    // ==========================================
    // Free Memory
    // ==========================================

    void Free(unsigned char *fileData)
    {
        delete[] fileData;
    }
}
