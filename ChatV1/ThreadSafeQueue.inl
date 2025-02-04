#include "ThreadSafeQueue.h"

template <typename T>
bool ThreadSafeQueue<T>::empty()
{
    std::lock_guard<std::mutex> lock(queueMutex);
    return queueData.empty();
}

template <typename T>
void ThreadSafeQueue<T>::push(T itemData)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    queueData.push(itemData);
}

template <typename T>
T ThreadSafeQueue<T>::pop()
{
    std::lock_guard<std::mutex> lock(queueMutex);
    T itemData = queueData.front();
    queueData.pop();
    return itemData;
}
