#include "Cryptography.h"
#include <cstdio>

void Cryptography::applyXOR(char* data, size_t size, const std::string& key) {
    if (key.empty()) return;
    for (size_t i = 0; i < size; ++i) {
        data[i] ^= key[i % key.length()];
    }
}

std::string Cryptography::generateHash(const std::string& password) {
    unsigned long hash = 5381;
    for (char c : password) {
        hash = ((hash << 5) + hash) + c;
    }
    char hexStr[17];
    snprintf(hexStr, sizeof(hexStr), "%016lx", hash);
    return std::string(hexStr);
}