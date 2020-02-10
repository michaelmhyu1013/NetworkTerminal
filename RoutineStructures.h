#pragma once

#include "ConnectionConfigurations.h"
#include "WSAEvents.h"
#include <queue>
#include <stdio.h>
#include <Windows.h>

struct AcceptStruct
{
    SOCKET                   acceptSocketDescriptor; // server's socket that handles newly created client connection
    SOCKET                   listenSocketDescriptor; // server's socket for listening to incoming client connect
    WSAEvents                *events;
    ConnectionConfigurations *connConfig;
    char                     *outputBuffer;
};

struct UDPServerStruct
{
    SOCKET                   *listenSocketDescriptor; // server's socket for listening to incoming client connect
    ConnectionConfigurations *connConfig;
    WSAEvents                *events;
    char                     *outputBuffer;
    struct sockaddr_in       *client;

    UDPServerStruct(SOCKET *listenSocketDescriptor, ConnectionConfigurations *connConfig, WSAEvents *events
                    , char *outputBuffer, struct sockaddr_in *client)
        : listenSocketDescriptor(listenSocketDescriptor)
        , connConfig(connConfig)
        , events(events)
        , outputBuffer(outputBuffer)
        , client(client) {}
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
    WSAEvents                *events;
    char                     *outputBuffer;

    FileUploadStruct(std::wstring fileName, ConnectionConfigurations *connConfig
                     , WSAEvents *events, char *outputBuffer)
        : fileName(fileName)
        , connConfig(connConfig)
        , events(events)
        , outputBuffer(outputBuffer) {}
};
