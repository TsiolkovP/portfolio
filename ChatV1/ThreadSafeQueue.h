#pragma once

#include <string>
#include <queue>
#include <mutex>

template <typename T>
class ThreadSafeQueue
{
private:
    std::queue<T> queueData;
    std::mutex queueMutex;

public:
    bool empty();
    void push(T itemData);
    T pop();
};
