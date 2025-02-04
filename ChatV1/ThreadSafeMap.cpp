#include "ThreadSafeMap.h"

bool ThreadSafeMap::find(std::string stringData)
{
    std::lock_guard<std::mutex> lock(mapMutex);

    if (unordered_mapData.find(stringData) == unordered_mapData.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void ThreadSafeMap::insert(std::string stringData, int intData)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    unordered_mapData.insert(std::make_pair(stringData, intData));
}

void ThreadSafeMap::erase(std::string stringData)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    unordered_mapData.erase(stringData);
}

int ThreadSafeMap::get(std::string stringData)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    int intData = unordered_mapData[stringData];
    return intData;
}

int ThreadSafeMap::size()
{
    std::lock_guard<std::mutex> lock(mapMutex);
    return unordered_mapData.size();
}

std::vector<std::string> ThreadSafeMap::to_string_vector()
{
    std::vector<std::string> onlineList;
    std::stringstream unitedData;
    std::lock_guard<std::mutex> lock(mapMutex);

    for (auto &pair : unordered_mapData)
    {
        unitedData << "Username: " << pair.first << ", Socket: " << pair.second;
        onlineList.push_back(unitedData.str());
        unitedData.str("");
        unitedData.clear();
    }

    return onlineList;
}