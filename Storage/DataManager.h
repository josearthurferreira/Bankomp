#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include "DataStructure.h"
#include <vector>
#include <string>

class DataManager {
private:
    std::string filename;
    std::string cryptoKey;
public:
    DataManager(const std::string& file, const std::string& key);
    bool saveRecords(const std::vector<RecordData>& records);
    std::vector<RecordData> loadRecords();
};

#endif