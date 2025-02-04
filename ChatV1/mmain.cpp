#include "ExecutingCommand.h"
#include "ServerListener.h"

#include <thread>
#include <arpa/inet.h>

static const int PORT = 8044;

static bool connect_to_server(int socket, char *ip)
{
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (inet_pton(AF_INET, ip, &serverAddress.sin_addr) <= 0)
    {
        return false;
    }

    std::cout << "[ZC] Trying to connect to the server..." << std::endl;

    if (connect(socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool parse_typed(std::string typedStr, UserCommands &typedCommand, std::vector<std::string> &parameters)
{
    if (typedStr.substr(0, 5) == "send ")
    {
        typedCommand = SEND;
    }
    else if (typedStr.substr(0, 5) == "auth ")
    {
        typedCommand = AUTH;
    }
    else if (typedStr.substr(0, 4) == "help")
    {
        typedCommand = HELP;
        return true;
    }
    else if (typedStr.substr(0, 2) == "up")
    {
        typedCommand = UPDATE;
        return true;
    }
    else if (typedStr.substr(0, 4) == "exit")
    {
        typedCommand = EXIT;
        return true;
    }
    else
    {
        std::cout << "[ZC] Incorrect typed command. Use #help to see HELP PAGE." << std::endl;
        return false;
    }

    std::string firstSecondParams = typedStr.substr(5);
    firstSecondParams.erase(0, firstSecondParams.find_first_not_of(" "));

    if (firstSecondParams.empty())
    {
        std::cout << "[ZC] No parameters provided." << std::endl;
        return false;
    }

    std::string firstParam;
    std::string secondParam;
    int firstSpacePos = 0;
    firstSpacePos = firstSecondParams.find(' ');

    if (firstSpacePos == std::string::npos)
    {
        firstParam = firstSecondParams;
    }
    else
    {
        firstParam = firstSecondParams.substr(0, firstSpacePos);
        secondParam = firstSecondParams.erase(0, firstSpacePos + 1);
    }

    parameters.push_back(firstParam);
    secondParam.erase(0, secondParam.find_first_not_of(" "));

    if (!secondParam.empty())
    {
        int lastSymbolPos = secondParam.find_last_not_of(" \t\n\r\f\v");

        if (lastSymbolPos != std::string::npos)
        {
            secondParam.erase(lastSymbolPos + 1);
        }

        parameters.push_back(secondParam);
    }

    return true;
}

static void write_messages_from_interlocutors(ThreadSafeQueue<CommandData> &recievedData)
{
    while (!recievedData.empty())
    {
        CommandData interlocutorMessage = recievedData.pop();

        if (interlocutorMessage.text == answer200)
        {
            std::cout << "[ZC] The message " << interlocutorMessage.messageID << " was successfully sent to the interlocutor." << std::endl;
        }
        else if (interlocutorMessage.text == answer300)
        {
            std::cout << "[ZC] The message " << interlocutorMessage.messageID << " was successfully sent to the server, the interlocutor is offline." << std::endl;
        }
        else
        {
            std::cout << interlocutorMessage.SRC << ": " << interlocutorMessage.text << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Welcome to the ZoomerChat!" << std::endl;
    std::cout << "Use #help to see HELP PAGE." << std::endl;

    char *ip;

    if (argc == 2)
    {
        ip = argv[1];
        std::cout << "[ZC] Loaded from console parameter: " << ip << std::endl;
    }
    else
    {
        std::cerr << "[ZC] There are no parameters or too many parameters from console. Closing the program..." << std::endl;
        return 1;
    }

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (connect_to_server(clientSocket, ip))
    {
        std::cout << "[ZC] Successfully connected to the server!" << std::endl;
    }
    else
    {
        std::cerr << "[ZC] The server is unavailable! Maybe IP-adress is wrong? Closing the program..." << std::endl;
        close(clientSocket);
        return 1;
    }

    bool running = true;
    std::atomic<bool> isConnectionDropped = false;
    uint32_t messageID = 0;
    std::string SRC;
    std::string typedStr;
    std::vector<std::string> parameters;
    UserCommands typedCommand;
    ThreadSafeQueue<CommandData> messages;
    std::thread ServerListener(listen_server, clientSocket, std::ref(messages), std::ref(isConnectionDropped));

    while (running && !isConnectionDropped)
    {
        typedStr.clear();
        parameters.clear();
        write_messages_from_interlocutors(messages);
        std::cout << "#";
        std::getline(std::cin, typedStr);

        if (parse_typed(typedStr, typedCommand, parameters))
        {
            execute_command(clientSocket, messageID, SRC, typedCommand, parameters, running);
        }
    }

    if (isConnectionDropped)
    {
        std::cout << "[ZC] Connection with server was dropped." << std::endl;
    }

    write_messages_from_interlocutors(messages);
    std::cout << "[ZC] The programm is closing..." << std::endl;
    std::cout << "[ZC] Goodbye!" << std::endl;
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    ServerListener.join();

    return 0;
}
