#pragma once

#include "ConnectionConfigurations.h"
#include "WSAEvents.h"
#include <queue>
#include <stdio.h>
#include <windows.h>

struct AcceptStruct
{
    SOCKET    acceptSocketDescriptor; // server's socket that handles newly created client connection
    SOCKET    listenSocketDescriptor; // server's socket for listening to incoming client connect
    WSAEvents *events;
};

struct SendStruct
{
    SOCKET                   clientSocketDescriptor; // client's socket to bind to host
    ConnectionConfigurations *connConfig;
    WSAEvents                *events;
    char                     *outputBuffer;

    SendStruct(ConnectionConfigurations *connConfig, WSAEvents *events,
               char *outputBuffer)
        : connConfig(connConfig)
        , events(events)
        , outputBuffer(outputBuffer) {}
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
