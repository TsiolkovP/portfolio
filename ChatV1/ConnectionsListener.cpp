#include "ConnectionsListener.h"

static const int MAX_CLIENTS_COUNT = 30;

static void write_debug(std::stringstream &debugMessage, ThreadSafeQueue<std::string> &debugData)
{
    debugData.push(debugMessage.str());
    debugMessage.str("");
    debugMessage.clear();
}

static bool send_command_to_client(int socket, uint32_t messageID, std::string SRC, std::string DST, std::string message)
{
    auto start = std::chrono::steady_clock::now();

    for (;;)
    {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> timeElapsed = now - start;

        if (send_ProtocolCommand(socket, 0, messageID, SRC, DST, message))
        {
            return true;
        }
        if (timeElapsed.count() >= 3.0)
        {
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

static void start_session(int sessionSocket, ThreadSafeMap &clients, ThreadSafeQueue<std::string> &debugData, CommandStorage &storage)
{
    std::stringstream debugMessage;
    uint16_t type;
    uint32_t messageID;
    std::string SRC;
    std::string sessionSRC;
    std::string DST;
    std::string message;
    debugMessage << "[ZC] New connection with socket " << sessionSocket << " was accepted.";
    write_debug(debugMessage, debugData);

    if (clients.size() == MAX_CLIENTS_COUNT)
    {
        close(sessionSocket);
        debugMessage << "[ZC] The client with socket " << sessionSocket << " was dropped. Online is more than 30.";
        write_debug(debugMessage, debugData);
    }
    else
    {
        while (recieve_ProtocolCommand(sessionSocket, type, messageID, SRC, DST, message))
        {
            if (type == 0)
            {
                if (SRC == DST)
                {
                    debugMessage << "[ZC] Error: The user " << SRC << " is trying to send the command to himself. Skipping the command.";
                    write_debug(debugMessage, debugData);
                    continue;
                }
                if (!clients.find(SRC))
                {
                    sessionSRC = SRC;
                    clients.insert(sessionSRC, sessionSocket);
                    debugMessage << "[ZC] The client " << sessionSRC << " was connected.";
                    write_debug(debugMessage, debugData);

                    if (!storage.is_empty())
                    {
                        storage.send_commands_from_storage(sessionSocket, SRC);
                        debugMessage << "[ZC] The client " << sessionSRC << " was gotten the commands from storage.";
                        write_debug(debugMessage, debugData);
                    }
                }
                else if (clients.get(SRC) != sessionSocket)
                {
                    debugMessage << "[ZC] Error: The user " << SRC << " already exists. Disconnecting the connection.";
                    write_debug(debugMessage, debugData);
                    break;
                }

                bool isSendedToDST = false;

                if (clients.find(DST))
                {
                    int DSTSocket = clients.get(DST);

                    if (send_command_to_client(DSTSocket, messageID, SRC, DST, message))
                    {
                        isSendedToDST = true;
                    }
                }
                if (!isSendedToDST)
                {
                    send_ProtocolCommand(sessionSocket, 1, messageID, SRC, DST, answer300);
                    storage.insert_command(messageID, SRC, DST, message);
                    debugMessage << "[ZC] The command from " << SRC << " with ID " << messageID << " was gotten and inserted into the storage.";
                    write_debug(debugMessage, debugData);
                }
            }
            if ((type == 1) && (message == answer200))
            {
                int senderSocket = clients.get(SRC);
                send_ProtocolCommand(senderSocket, 1, messageID, SRC, DST, answer200);
                debugMessage << "[ZC] The command from " << SRC << " with ID " << messageID << " to " << DST << " was succesfully sended.";
                write_debug(debugMessage, debugData);
            }
        }

        shutdown(sessionSocket, SHUT_RDWR);
        close(sessionSocket);
        clients.erase(sessionSRC);
        debugMessage << "[ZC] The client with socket " << sessionSocket << " was disconnected.";
        write_debug(debugMessage, debugData);
    }
}

void listen_connections(int &serverSocket, ServerConnectionData &currentRunData, ThreadSafeMap &clients, ThreadSafeQueue<std::string> &debugData, std::atomic<bool> &running)
{
    int clientSocket;
    std::vector<int> sessionsSockets;
    std::vector<std::thread> sessions;
    std::stringstream debugMessage;
    CommandStorage storage;

    while (running)
    {
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&currentRunData.address, (socklen_t *)&currentRunData.addrlen)) < 0)
        {
            debugMessage << "[ZC] Can't accept socket.";
            write_debug(debugMessage, debugData);
            continue;
        }
        if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&currentRunData.timeout, sizeof(currentRunData.timeout)) < 0)
        {
            shutdown(clientSocket, SHUT_RDWR);
            close(clientSocket);
            debugMessage << "[ZC] Can't set timeout for socket.";
            write_debug(debugMessage, debugData);
            continue;
        }

        sessionsSockets.push_back(clientSocket);
        sessions.emplace_back(start_session, clientSocket, std::ref(clients), std::ref(debugData), std::ref(storage));
    }

    for (int i = 0; i < sessions.size(); i++)
    {
        shutdown(sessionsSockets[i], SHUT_RDWR);

        if (sessions[i].joinable())
        {
            sessions[i].join();
        }
    }
}