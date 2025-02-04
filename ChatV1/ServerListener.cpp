#include "ServerListener.h"

static bool recieve_and_send_answer(int socket, CommandData &interlocutorMessage)
{
    uint16_t len;
    uint16_t type;
    uint32_t messageID;
    std::string SRC;
    std::string DST;
    std::string message;

    if (recieve_ProtocolCommand(socket, type, messageID, SRC, DST, message))
    {
        if (type == 0)
        {
            if (!send_ProtocolCommand(socket, 1, messageID, SRC, DST, answer200))
            {
                return false;
            }
        }
        else if (!(type == 1))
        {
            return false;
        }

        interlocutorMessage.SRC = SRC;
        interlocutorMessage.text = message;
        interlocutorMessage.messageID = messageID;
        return true;
    }
    else
    {
        return false;
    }
}

void listen_server(int socket, ThreadSafeQueue<CommandData> &recievedData, std::atomic<bool> &isConnectionDropped)
{
    CommandData interlocutorMessage;

    while (recieve_and_send_answer(socket, interlocutorMessage))
    {
        recievedData.push(interlocutorMessage);
    }

    isConnectionDropped = true;
}
