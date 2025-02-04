#pragma once

#include "ProtocolCommand.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeQueue.inl"

#include <iostream>
#include <atomic> 

struct CommandData
{
    uint32_t messageID;
    std::string SRC;
    std::string text;
};

void listen_server(int socket, ThreadSafeQueue<CommandData> &recievedData, std::atomic<bool> &isConnectionDropped);
