#pragma once

#include "ConnectionConfigurations.h"
#include "WSAEvents.h"
#include <queue>
#include <stdio.h>
#include <windows.h>

struct AcceptStruct
{
    SOCKET                   acceptSocketDescriptor; // server's socket that handles newly created client connection
    SOCKET                   listenSocketDescriptor; // server's socket for listening to incoming client connect
    WSAEvents                *events;
    ConnectionConfigurations *connConfig;
    char                     *outputBuffer;
    bool                     *isConnected;
};

struct SendStruct
{
    SOCKET                   clientSocketDescriptor; // client's socket to bind to host
    ConnectionConfigurations *connConfig;
    WSAEvents                *events;
    char                     *outputBuffer;
    struct sockaddr_in       *server;
    bool                     *isConnected;
    SendStruct(ConnectionConfigurations *connConfig, WSAEvents *events,
               char *outputBuffer, struct sockaddr_in *server, bool *isConnected)
        : connConfig(connConfig)
        , events(events)
        , outputBuffer(outputBuffer)
        , server(server)
        , isConnected(isConnected) {}
};

struct FileUploadStruct
{
    std::wstring             fileName;
    ConnectionConfigurations *connConfig;
    char                     *outputBuffer;
    WSAEvents                *events;
    FileUploadStruct(std::wstring fileName, ConnectionConfigurations *connConfig
                     , WSAEvents *events, char *outputBuffer)
        : fileName(fileName)
        , connConfig(connConfig)
        , events(events)
        , outputBuffer(outputBuffer) {}
};
