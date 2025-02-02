#include "crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <vector>

const int AES_KEY_LENGTH = 256;

std::string encryptAES(const std::string& plaintext, const std::string& key) {

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Failed to create cipher context!" << std::endl;
        return "";
    }

    std::vector<unsigned char> iv = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P' };

    std::vector<unsigned char> ciphertext(plaintext.size() + 16);
    int len = 0, ciphertext_len = 0;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)key.c_str(), iv.data());
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, (unsigned char*)plaintext.c_str(), plaintext.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(ciphertext.begin(), ciphertext.begin() + ciphertext_len);
}


std::string decryptAES(const std::string& ciphertext, const std::string& key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Failed to create cipher context!" << std::endl;
        return "";
    }

    std::vector<unsigned char> iv = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P' };

    std::vector<unsigned char> decrypted(ciphertext.size());
    int len = 0, decrypted_len = 0;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (unsigned char*)key.c_str(), iv.data());
    EVP_DecryptUpdate(ctx, decrypted.data(), &len, (unsigned char*)ciphertext.c_str(), ciphertext.size());
    decrypted_len = len;
    EVP_DecryptFinal_ex(ctx, decrypted.data() + len, &len);
    decrypted_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(decrypted.begin(), decrypted.begin() + decrypted_len);
}

