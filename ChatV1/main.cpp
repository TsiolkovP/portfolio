#include "ConnectionsListener.h"

static const int PORT = 8044;
static const int MAX_CLIENTS_COUNT = 30;

enum UserCommands
{
    HELP,
    ONLINE,
    DEBUG,
    EXIT
};

static bool parse_typed(std::string typedStr, UserCommands &typedCommand)
{
    if (typedStr.substr(0, 4) == "help")
    {
        typedCommand = HELP;
        return true;
    }
    if (typedStr.substr(0, 5) == "debug")
    {
        typedCommand = DEBUG;
        return true;
    }
    if (typedStr.substr(0, 6) == "online")
    {
        typedCommand = ONLINE;
        return true;
    }
    if (typedStr.substr(0, 4) == "exit")
    {
        typedCommand = EXIT;
        return true;
    }

    std::cout << "[ZC] Incorrect typed command. Use #help to see HELP PAGE." << std::endl;
    return false;
}

static void execute_command(UserCommands typedCommand, std::atomic<bool> &running, ThreadSafeQueue<std::string> &debugData, ThreadSafeMap &clients)
{
    switch (typedCommand)
    {
    case HELP:
    {
        std::cout << "------------------" << std::endl;
        std::cout << "HELP PAGE" << std::endl;
        std::cout << "------------------" << std::endl;
        std::cout << "COMMAND - DEFINITION" << std::endl;
        std::cout << "------------------" << std::endl;
        std::cout << "help - see the HELP PAGE." << std::endl;
        std::cout << "debug - see the debug messages." << std::endl;
        std::cout << "online - see the online list of the users." << std::endl;
        std::cout << "exit - close the application." << std::endl;
        std::cout << "------------------" << std::endl;
        break;
    }
    case DEBUG:
    {
        if (debugData.empty())
        {
            std::cout << "[ZC] The debug buffer is empty." << std::endl;
        }
        while (!debugData.empty())
        {
            std::cout << debugData.pop() << std::endl;
        }

        break;
    }
    case ONLINE:
    {
        std::vector<std::string> onlineList = clients.to_string_vector();

        if (onlineList.size() == 0)
        {
            std::cout << "[ZC] There is no one online." << std::endl;
        }
        else
        {
            std::cout << "------------------" << std::endl;
            std::cout << "ONLINE" << std::endl;
            std::cout << "------------------" << std::endl;

            for (auto &str : onlineList)
            {
                std::cout << str << std::endl;
            }

            std::cout << "------------------" << std::endl;
        }

        break;
    }
    case EXIT:
    {
        running = false;
        break;
    }
    }
}

int main()
{
    std::cout << "Welcome to the server of the ZoomerChat!" << std::endl;
    std::cout << "Use #help to see HELP PAGE." << std::endl;

    int serverSocket;
    int opt = 1;
    std::atomic<bool> running(true);
    std::string typedStr;
    ThreadSafeQueue<std::string> debugData;
    ThreadSafeMap clients;
    UserCommands typedCommand;

    ServerConnectionData currentRunData;
    currentRunData.address.sin_family = AF_INET;
    currentRunData.address.sin_addr.s_addr = INADDR_ANY;
    currentRunData.address.sin_port = htons(PORT);
    currentRunData.addrlen = sizeof(currentRunData.address);
    currentRunData.timeout.tv_sec = 240;
    currentRunData.timeout.tv_usec = 0;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "[ZC] Socket failed." << std::endl;
        return 1;
    }
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        std::cerr << "[ZC] Socket failed." << std::endl;
        return 1;
    }
    if (bind(serverSocket, (struct sockaddr *)&currentRunData.address, currentRunData.addrlen) < 0)
    {
        std::cerr << "[ZC] Bind failed." << std::endl;
        return 1;
    }
    if (listen(serverSocket, MAX_CLIENTS_COUNT + 1) < 0)
    {
        std::cerr << "[ZC] Listen failed." << std::endl;
        return 1;
    }

    std::thread ConnectionsListener(listen_connections, std::ref(serverSocket), std::ref(currentRunData), std::ref(clients), std::ref(debugData), std::ref(running));

    while (running)
    {
        typedStr.clear();
        std::cout << "#";
        std::getline(std::cin, typedStr);

        if (parse_typed(typedStr, typedCommand))
        {
            execute_command(typedCommand, running, debugData, clients);
        }
    }

    std::cout << "[ZC] The programm is closing..." << std::endl;
    std::cout << "[ZC] Goodbye!" << std::endl;
    shutdown(serverSocket, SHUT_RD);
    close(serverSocket);
    ConnectionsListener.join();

    return 0;
}