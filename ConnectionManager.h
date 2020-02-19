#pragma once

#include "Client.h"
#include "ConnectionConfigurations.h"
#include "Definitions.h"
#include "RoutineStructures.h"
#include "Server.h"
#include "TimeClock.h"
#include "WindowsThreadService.h"
#include "WSAEvents.h"
#include <queue>
#include <winsock2.h>

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
    void cleanUp();

protected:
    WORD               wVersionRequested = MAKEWORD(2, 2);
    WSADATA            WSAData;
    struct sockaddr_in client, server;
    struct hostent     *hp;
    AcceptStruct       *asInfo;
    SendStruct         *ss;
    WSAEvents          *events;
    bool               clientConnected{ 0 }, serverConnected{ 0 };
    int                n;
    char               *outputBuffer = new char[MAX_FILE_SIZE];

private:
    int bindServer(ConnectionConfigurations *connConfig);
    int bindUDPClient(sockaddr_in &client, SendStruct *ss);
    int configureClientAddressStructures(ConnectionConfigurations *connConfig, SendStruct *ss);

    Client *clnt;
    Server *serv;

    ~ConnectionManager();
};
