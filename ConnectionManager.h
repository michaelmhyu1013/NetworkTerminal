#pragma once

#include "Connection.h"
#include "ConnectionConfigurations.h"
#include "RoutineStructures.h"
#include "WindowsThreadService.h"
#include "WSAEvents.h"
#include <queue>
#include <winsock2.h>

#define MAX_FILE_SIZE    20000000

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


protected:
//    ConnectionConfigurations connConfig;
    WORD                 wVersionRequested = MAKEWORD(2, 2);
    WindowsThreadService *threadService;
    WSADATA              WSAData;
    HANDLE               acceptThread, readThread, writeThread, connectThread, fileThread;
    struct sockaddr_in   client, server;
    struct hostent       *hp;
    WSAEvents            *events;
    DWORD                acceptThreadID, readThreadID, writeThreadID, connectThreadID, fileThreadID;
    AcceptStruct         *asInfo;
    int                  n, ns, bytes_to_read, port, err;
    char                 *host, *bp, **pptr;

    char                 *outputBuffer = new char[MAX_FILE_SIZE];

private:
    int bindServer(ConnectionConfigurations *connConfig);
    int configureClientAddressStructures(ConnectionConfigurations *connConfig, SendStruct *ss);
};
