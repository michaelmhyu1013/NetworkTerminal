#pragma once
#include "Socket.h"

#include <string>
#include <windows.h >
#include <winsock2.h>

class Connection
{
private:
    std::string ip;
    int         port;
    Socket      socket;

public:
    Connection();
    void configureSocket();
};
