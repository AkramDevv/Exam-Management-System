#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>

std::string encryptAES(const std::string& plaintext, const std::string& key);
std::string decryptAES(const std::string& ciphertext, const std::string& key);

#endif