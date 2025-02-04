#include "ExecutingCommand.h"

static bool check_auth_data(std::vector<std::string> parameters)
{
    if (parameters.size() != 1)
    {
        std::cout << "[ZC] The invalid parameters. Use #auth USERNAME." << std::endl;
        return false;
    }
    else if ((parameters[0].length() == 0) || (parameters[0].length() > 8))
    {
        std::cout << "[ZC] The invalid username. It should be from 1 to 8 letters." << std::endl;
        return false;
    }

    return true;
}

static bool check_send_data(std::vector<std::string> parameters)
{
    if (parameters.size() != 2)
    {
        std::cout << "[ZC] The invalid number of parameters. Use #send USERNAME TEXT." << std::endl;
        return false;
    }
    else if ((parameters[0].length() == 0) || (parameters[0].length() > 8))
    {
        std::cout << "[ZC] The invalid DST. It should be from 1 to 8 letters." << std::endl;
        return false;
    }
    else if ((parameters[1].length() == 0) || (parameters[1].length() > 1000))
    {
        std::cout << "[ZC] The invalid message. It should be from 1 to 1000 letters." << std::endl;
        return false;
    }

    return true;
}

void execute_command(int socket, uint32_t &messageID, std::string &SRC, UserCommands typedCommand, std::vector<std::string> parameters, bool &running)
{
    switch (typedCommand)
    {
    case AUTH:
    {
        if (SRC.size() != 0)
        {
            std::cout << "[ZC] You are already logged in." << std::endl;
            break;
        }
        if (check_auth_data(parameters))
        {
            SRC = parameters[0];
            std::cout << "[ZC] Succesfull authorization!" << std::endl;
        }
        break;
    }
    case SEND:
    {
        if (SRC.size() != 0)
        {
            if (check_send_data(parameters))
            {
                std::string DST = parameters[0];
                std::string message = parameters[1];
                if (send_ProtocolCommand(socket, 0, messageID, SRC, DST, message))
                {
                    // Чтобы сервер успел прислать ответ к следующей итерации цикла ввода команд пользователем в main.cpp
                    sleep(1);
                    messageID = messageID + 1;
                }
                else
                {
                    std::cout << "[ZC] The server is offline." << std::endl;
                    running = false;
                }
            }
        }
        else
        {
            std::cout << "[ZC] You are not logged in. Use #auth USERNAME." << std::endl;
        }

        break;
    }
    case HELP:
    {
        std::cout << "------------------" << std::endl;
        std::cout << "HELP PAGE" << std::endl;
        std::cout << "------------------" << std::endl;
        std::cout << "COMMAND - DEFINITION" << std::endl;
        std::cout << "------------------" << std::endl;
        std::cout << "help - see the HELP PAGE." << std::endl;
        std::cout << "auth USERNAME - log in to the chat." << std::endl;
        std::cout << "send USERNAME TEXT - send message to other user." << std::endl;
        std::cout << "up - load the messages from the buffer, which were sent by other users." << std::endl;
        std::cout << "exit - close the application." << std::endl;
        std::cout << "------------------" << std::endl;
        break;
    }
    // Так-как отправка сообщений от сервера пользователю и прием команд от пользователя выполняются однопоточно и последовательно в цикле,
    // добавляем команду завершения итерации цикла для вывода сообщений других юзеров в следующей итерации.
    case UPDATE:
    {
        std::cout << "[ZC] The messages were loaded from the buffer." << std::endl;
        break;
    }
    case EXIT:
    {
        running = false;
        break;
    }
    }
}
