#pragma once

#include "ProtocolCommand.h"

#include <unordered_map>
#include <mutex>

struct SavedCommand
{
    uint16_t type;
    uint32_t messageID;
    std::string SRC;
    std::string message;
};

class CommandStorage
{
private:
    mutable std::mutex storageMutex;
    std::unordered_map<std::string, std::vector<SavedCommand>> commands;

public:
    void insert_command(uint32_t messageID, std::string SRC, std::string DST, std::string message);
    void erase_commands(std::string DST);
    std::vector<SavedCommand> take_commands(std::string DST);
    bool is_empty();
    bool send_commands_from_storage(int socket, std::string DST);
};
