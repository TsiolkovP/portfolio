#include "CommandStorage.h"

static bool send_and_recieve_answer(int socket, uint32_t messageID, std::string SRC, std::string DST, std::string message)
{
    uint16_t recievedType;
    uint32_t recievedMessageID;
    std::string recievedSRC;
    std::string recievedDST;
    std::string recievedMessage;

    if (send_ProtocolCommand(socket, 0, messageID, SRC, DST, message))
    {
        if (recieve_ProtocolCommand(socket, recievedType, recievedMessageID, recievedSRC, recievedDST, recievedMessage))
        {
            if ((recievedType == 1) && (recievedMessageID == messageID) && (recievedSRC == SRC) && (recievedDST == DST) && (recievedMessage == answer200))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return false;
}

void CommandStorage::insert_command(uint32_t messageID, std::string SRC, std::string DST, std::string message)
{
    std::lock_guard<std::mutex> lock(storageMutex);
    SavedCommand newCommand;
    newCommand.type = 2;
    newCommand.messageID = messageID;
    newCommand.message = message;
    newCommand.SRC = SRC;

    if (!(commands.find(DST) == commands.end()))
    {
        commands[DST].push_back(newCommand);
    }
    else
    {
        std::vector<SavedCommand> newCommands;
        newCommands.push_back(newCommand);
        commands.insert(std::make_pair(DST, newCommands));
    }
}

void CommandStorage::erase_commands(std::string DST)
{
    std::lock_guard<std::mutex> lock(storageMutex);
    commands.erase(DST);
}

std::vector<SavedCommand> CommandStorage::take_commands(std::string DST)
{
    std::lock_guard<std::mutex> lock(storageMutex);
    std::vector<SavedCommand> findedMessages;

    if (commands.find(DST) != commands.end())
    {
        findedMessages = commands[DST];
    }

    return findedMessages;
}

bool CommandStorage::is_empty()
{
    std::lock_guard<std::mutex> lock(storageMutex);

    if (commands.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandStorage::send_commands_from_storage(int socket, std::string DST)
{
    std::vector<SavedCommand> foundCommands = take_commands(DST);

    if (!foundCommands.empty())
    {
        for (int i = 0; i < foundCommands.size(); i++)
        {
            SavedCommand currentCommand = foundCommands[i];
            if (!send_and_recieve_answer(socket, currentCommand.messageID, currentCommand.SRC, DST, currentCommand.message))
            {
                return false;
            }
        }
    }

    erase_commands(DST);
    return true;
}