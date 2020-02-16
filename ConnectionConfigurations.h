#pragma once
#include <string>
#include <Windows.h>

struct ConnectionConfigurations
{
    int         connectionMode;
    int         connectionType;
    std::string *ip;
    int         port = 7777;
    int         packetSize;
    int         transmissions;
    int         socketConnectionType;
    DWORD       totalBytes;
    enum CONNECTION_TYPE
    {
        UDP = 0,
        TCP = 1,
    };

    enum CONNECT_MODE
    {
        CLIENT = 0,
        SERVER = 1,
    };
};
