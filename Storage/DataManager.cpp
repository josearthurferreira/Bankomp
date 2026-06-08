#include "DataManager.h"
#include "Cryptography.h"
#include <fstream>

DataManager::DataManager(const std::string& file, const std::string& key) 
    : filename(file), cryptoKey(key) {}

bool DataManager::saveRecords(const std::vector<RecordData>& records) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    for (auto record : records) {
        Cryptography::applyXOR(reinterpret_cast<char*>(&record), sizeof(RecordData), cryptoKey);
        file.write(reinterpret_cast<const char*>(&record), sizeof(RecordData));
    }
    return true;
}

std::vector<RecordData> DataManager::loadRecords() {
    std::vector<RecordData> records;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return records;

    RecordData record;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(RecordData))) {
        Cryptography::applyXOR(reinterpret_cast<char*>(&record), sizeof(RecordData), cryptoKey);
        records.push_back(record);
    }
    return records;
}