#pragma once

#include "ThreadSafeMap.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeQueue.inl"
#include "CommandStorage.h"

#include <unistd.h>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

struct ServerConnectionData
{
    struct sockaddr_in address;
    struct timeval timeout;
    int addrlen;
};

void listen_connections(int &serverSocket, ServerConnectionData &currentRunData, ThreadSafeMap &clients, ThreadSafeQueue<std::string> &debugData, std::atomic<bool> &running);