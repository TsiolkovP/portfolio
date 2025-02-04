#pragma once

#include "ProtocolCommand.h"

#include <iostream>
#include <unistd.h>

enum UserCommands
{
    SEND,
    AUTH,
    HELP,
    UPDATE,
    EXIT
};

void execute_command(int socket, uint32_t &messageID, std::string &SRC, UserCommands typedCommand, std::vector<std::string> parameters, bool &running);