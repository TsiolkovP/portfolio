#include "ProtocolCommand.h"

static const uint16_t LENGTH_OF_COMMAND = 24;

std::string answer200 = "200";
std::string answer300 = "300";

struct ProtocolCommand
{
    uint16_t len;
    uint16_t type;
    uint32_t messageID;
    char SRC[8];
    char DST[8];
};

bool send_ProtocolCommand(int socket, uint16_t type, uint32_t messageID, std::string SRC, std::string DST, std::string message)
{
    if (!(type == 0 || type == 1 || type == 2))
    {
        return false;
    }
    if (SRC.size() > 8 || DST.size() > 8)
    {
        return false;
    }
    if (message.size() > 1000)
    {
        return false;
    }

    ProtocolCommand command;
    command.len = LENGTH_OF_COMMAND + message.size();
    command.type = type;
    command.messageID = messageID;
    std::memset(command.SRC, 0, sizeof(command.SRC));
    std::memset(command.DST, 0, sizeof(command.DST));
    SRC.copy(command.SRC, SRC.size());
    DST.copy(command.DST, DST.size());

    if (send(socket, &command, LENGTH_OF_COMMAND, 0) == -1)
    {
        return false;
    }
    if (send(socket, message.c_str(), message.size(), 0) == -1)
    {
        return false;
    }

    return true;
}

bool recieve_ProtocolCommand(int socket, uint16_t &type, uint32_t &messageID, std::string &SRC, std::string &DST, std::string &message)
{
    ProtocolCommand command;

    if (recv(socket, &command, LENGTH_OF_COMMAND, 0) <= 0)
    {
        return false;
    }

    int messageSize = command.len - LENGTH_OF_COMMAND;
    std::vector<char> charMessage(messageSize);

    if (recv(socket, charMessage.data(), messageSize, 0) <= 0)
    {
        return false;
    }

    type = command.type;
    messageID = command.messageID;
    SRC = std::string(command.SRC, 8);
    DST = std::string(command.DST, 8);
    message = std::string(charMessage.data(), messageSize);

    return true;
}
