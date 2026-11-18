#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <memory>
#include <algorithm>
#include <limits>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

// ==========================================
// Configuration
// ==========================================

namespace Crypto
{
    constexpr int KEY_SIZE = 32; // AES-256
    constexpr int SALT_SIZE = 16;
    constexpr int NONCE_SIZE = 12;
    constexpr int TAG_SIZE = 16;

    constexpr int PBKDF2_ITERATIONS = 600000;

    constexpr char MAGIC[] = "RAYENC01";
    constexpr size_t HEADER_SIZE = 8 + SALT_SIZE + NONCE_SIZE;

    using Bytes = std::vector<unsigned char>;

    using CipherContext =
        std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)>;

    // ======================================
    // File I/O
    // ======================================

    bool ReadFile(const std::string &path, Bytes &data)
    {
        std::ifstream file(path, std::ios::binary);

        if (!file)
            return false;

        file.seekg(0, std::ios::end);
        auto size = file.tellg();

        if (size < 0)
            return false;

        if (static_cast<uintmax_t>(size) >
            static_cast<uintmax_t>(std::numeric_limits<size_t>::max()))
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

    bool WriteFile(const std::string &path, const Bytes &data)
    {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);

        if (!file)
            return false;

        if (!data.empty())
        {
            file.write(
                reinterpret_cast<const char *>(data.data()),
                static_cast<std::streamsize>(data.size()));
        }

        return file.good();
    }

    // ======================================
    // Key derivation
    // ======================================

    bool DeriveKey(
        const std::string &password,
        const unsigned char *salt,
        std::array<unsigned char, KEY_SIZE> &key)
    {
        if (password.empty() ||
            password.size() > static_cast<size_t>(
                                  std::numeric_limits<int>::max()))
            return false;

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

    // ======================================
    // Encrypt
    // ======================================

    bool Encrypt(
        const std::string &inputPath,
        const std::string &outputPath,
        const std::string &password)
    {
        Bytes plain;

        if (!ReadFile(inputPath, plain))
        {
            std::cerr << "Failed to read input file.\n";
            return false;
        }

        // Generate random salt and nonce.

        std::array<unsigned char, SALT_SIZE> salt{};
        std::array<unsigned char, NONCE_SIZE> nonce{};

        if (RAND_bytes(salt.data(), SALT_SIZE) != 1 ||
            RAND_bytes(nonce.data(), NONCE_SIZE) != 1)
        {
            std::cerr << "Random generation failed.\n";
            return false;
        }

        // Derive encryption key.

        std::array<unsigned char, KEY_SIZE> key{};

        if (!DeriveKey(password, salt.data(), key))
        {
            std::cerr << "Key derivation failed.\n";
            return false;
        }

        CipherContext ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);

        if (!ctx)
            return false;

        if (EVP_EncryptInit_ex(
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

        if (EVP_EncryptInit_ex(
                ctx.get(),
                nullptr,
                nullptr,
                key.data(),
                nonce.data()) != 1)
            return false;

        // Build authenticated header.

        Bytes header(HEADER_SIZE);

        std::copy(
            MAGIC,
            MAGIC + 8,
            header.begin());

        std::copy(
            salt.begin(),
            salt.end(),
            header.begin() + 8);

        std::copy(
            nonce.begin(),
            nonce.end(),
            header.begin() + 8 + SALT_SIZE);

        // Authenticate header as AAD.

        int len = 0;

        if (EVP_EncryptUpdate(
                ctx.get(),
                nullptr,
                &len,
                header.data(),
                static_cast<int>(header.size())) != 1)
            return false;

        // Encrypt data in chunks.

        Bytes encrypted(plain.size() + EVP_MAX_BLOCK_LENGTH);

        size_t offset = 0;
        size_t written = 0;

        while (offset < plain.size())
        {
            int chunk = static_cast<int>(
                std::min(
                    plain.size() - offset,
                    static_cast<size_t>(std::numeric_limits<int>::max())));

            if (EVP_EncryptUpdate(
                    ctx.get(),
                    encrypted.data() + written,
                    &len,
                    plain.data() + offset,
                    chunk) != 1)
                return false;

            offset += chunk;
            written += static_cast<size_t>(len);
        }

        if (EVP_EncryptFinal_ex(
                ctx.get(),
                encrypted.data() + written,
                &len) != 1)
            return false;

        written += static_cast<size_t>(len);
        encrypted.resize(written);

        // Get authentication tag.

        std::array<unsigned char, TAG_SIZE> tag{};

        if (EVP_CIPHER_CTX_ctrl(
                ctx.get(),
                EVP_CTRL_GCM_GET_TAG,
                TAG_SIZE,
                tag.data()) != 1)
            return false;

        // Output format:
        // [HEADER][CIPHERTEXT][TAG]

        Bytes output;

        output.reserve(header.size() + encrypted.size() + TAG_SIZE);

        output.insert(
            output.end(),
            header.begin(),
            header.end());

        output.insert(
            output.end(),
            encrypted.begin(),
            encrypted.end());

        output.insert(
            output.end(),
            tag.begin(),
            tag.end());

        if (!WriteFile(outputPath, output))
        {
            std::cerr << "Failed to write output file.\n";
            return false;
        }

        std::cout << "Encryption successful!\n";

        return true;
    }

    // ======================================
    // Decrypt
    // ======================================

    bool Decrypt(
        const std::string &inputPath,
        const std::string &outputPath,
        const std::string &password)
    {
        Bytes input;

        if (!ReadFile(inputPath, input))
        {
            std::cerr << "Failed to read input file.\n";
            return false;
        }

        if (input.size() < HEADER_SIZE + TAG_SIZE)
        {
            std::cerr << "Invalid encrypted file.\n";
            return false;
        }

        // Validate magic header.

        if (!std::equal(
                MAGIC,
                MAGIC + 8,
                input.begin()))
        {
            std::cerr << "Invalid file format.\n";
            return false;
        }

        // Extract salt and nonce.

        const unsigned char *salt = input.data() + 8;

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

        CipherContext ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);

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

        // Authenticate header.

        int len = 0;

        if (EVP_DecryptUpdate(
                ctx.get(),
                nullptr,
                &len,
                input.data(),
                static_cast<int>(HEADER_SIZE)) != 1)
            return false;

        // Decrypt data.

        Bytes decrypted(cipherSize + EVP_MAX_BLOCK_LENGTH);

        size_t offset = 0;
        size_t written = 0;

        while (offset < cipherSize)
        {
            int chunk = static_cast<int>(
                std::min(
                    cipherSize - offset,
                    static_cast<size_t>(std::numeric_limits<int>::max())));

            if (EVP_DecryptUpdate(
                    ctx.get(),
                    decrypted.data() + written,
                    &len,
                    cipherText + offset,
                    chunk) != 1)
                return false;

            offset += chunk;
            written += static_cast<size_t>(len);
        }

        // Set authentication tag.

        if (EVP_CIPHER_CTX_ctrl(
                ctx.get(),
                EVP_CTRL_GCM_SET_TAG,
                TAG_SIZE,
                const_cast<unsigned char *>(tag)) != 1)
            return false;

        // Verify authentication.
        // This fails if password or file contents are incorrect.

        if (EVP_DecryptFinal_ex(
                ctx.get(),
                decrypted.data() + written,
                &len) != 1)
        {
            std::cerr << "Authentication failed!\n";
            std::cerr << "Wrong password or corrupted file.\n";
            return false;
        }

        written += static_cast<size_t>(len);
        decrypted.resize(written);

        // Write only after successful authentication.

        if (!WriteFile(outputPath, decrypted))
        {
            std::cerr << "Failed to write output file.\n";
            return false;
        }

        std::cout << "Decryption successful!\n";

        return true;
    }
}

// ==========================================
// Main
// ==========================================
#include <cstdlib>

int main(int argc, char *argv[])
{
    using namespace Crypto;

    if (argc != 4)
    {
        std::cerr
            << "Usage:\n"
            << "  Encrypt: program e input output\n"
            << "  Decrypt: program d input output\n";

        return 1;
    }

    std::string mode = argv[1];
    std::string inputPath = argv[2];
    std::string outputPath = argv[3];

    if (inputPath == outputPath)
    {
        std::cerr << "Input and output paths must be different.\n";
        return 1;
    }

    const char *envPassword = "faihfoiahghasgaioghoahohgohirag";

    if (envPassword == nullptr || envPassword[0] == '\0')
    {
        std::cerr << "NOVELGAME_ASSET_KEY is not set.\n";
        return 1;
    }

    std::string password = envPassword;

    bool result = false;

    if (mode == "e")
    {
        result = Encrypt(inputPath, outputPath, password);
    }
    else if (mode == "d")
    {
        result = Decrypt(inputPath, outputPath, password);
    }
    else
    {
        std::cerr << "Unknown mode.\n";
        return 1;
    }

    std::fill(password.begin(), password.end(), '\0');

    return result ? 0 : 1;
}