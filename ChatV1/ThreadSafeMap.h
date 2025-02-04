#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>

class ThreadSafeMap
{
private:
    std::unordered_map<std::string, int> unordered_mapData;
    std::mutex mapMutex;

public:
    bool find(std::string stringData);
    void insert(std::string stringData, int intData);
    void erase(std::string stringData);
    int get(std::string stringData);
    int size();
    std::vector<std::string> to_string_vector();
};