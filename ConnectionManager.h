#pragma once

#include "ConnectionConfigurations.h"
#include "Definitions.h"
#include "RoutineStructures.h"
#include "TimeClock.h"
#include "WindowsThreadService.h"
#include "WSAEvents.h"
#include <queue>
#include <WinSock2.h>

class WindowsThreadService;

class ConnectionManager
{
public:
    ConnectionManager();
    void createUDPClient(ConnectionConfigurations *connConfig);
    void createTCPClient(ConnectionConfigurations *connConfig);
    void createUDPServer(ConnectionConfigurations *connConfig);
    void createTCPServer(ConnectionConfigurations *connConfig);
    void uploadFile(ConnectionConfigurations *connConfig, std::wstring fileName);

    inline TimeClock getClock() { return(this->clock); }

protected:
    WindowsThreadService *threadService;
    WSADATA              WSAData;
    HANDLE               acceptThread, readThread, writeThread, connectThread, fileThread;
    struct sockaddr_in   client, server;
    struct hostent       *hp;
    WSAEvents            *events;
    DWORD                acceptThreadID, readThreadID, writeThreadID, connectThreadID, fileThreadID;
    AcceptStruct         *asInfo;
    int                  n;

    TimeClock            clock;
    WORD                 wVersionRequested = MAKEWORD(2, 2);
    char                 *outputBuffer     = new char[MAX_FILE_SIZE];

private:
    int bindServer(ConnectionConfigurations *connConfig);
    int bindUDPClient(sockaddr_in &client, SendStruct *ss);
    int configureClientAddressStructures(ConnectionConfigurations *connConfig, SendStruct *ss);
};
