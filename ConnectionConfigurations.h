#pragma once
#include <string>
#include <Windows.h>

// TODO: change to singleton
struct ConnectionConfigurations
{
    int         connectionMode;
    int         connectionType;
    std::string *ip;
    int         port = 7777;
    int         packetSize;
    int         transmissions;
    int         socketConnectionType;
    DWORD       totalTransmissionsize;
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
