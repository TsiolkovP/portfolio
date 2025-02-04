#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

extern std::string answer200;
extern std::string answer300;

// Отправка комманды серверу

bool send_ProtocolCommand(int socket, uint16_t type, uint32_t messageID, std::string SRC, std::string DST, std::string message);

// Прием комманды от сервера

bool recieve_ProtocolCommand(int socket, uint16_t &type, uint32_t &messageID, std::string &SRC, std::string &DST, std::string &message);
