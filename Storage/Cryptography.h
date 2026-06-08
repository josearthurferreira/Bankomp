#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H
#include <string>

class Cryptography {
public:
    static void applyXOR(char* data, size_t size, const std::string& key);
    static std::string generateHash(const std::string& password);
};

#endif