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
    ConnectionConfigurations *connConfig;
    WSAEvents                *events;
    SOCKET                   clientSocketDescriptor; // client's socket to bind to host
    std::queue<std::string>  *outputBuffer;

    SendStruct(ConnectionConfigurations *connConfig, WSAEvents *events,
               std::queue<std::string> *outputBuffer)
        : connConfig(connConfig)
        , events(events)
        , outputBuffer(outputBuffer) {}
};

struct FileUploadStruct
{
    ConnectionConfigurations *connConfig;
    std::queue<std::string>  *outputBuffer;
    std::wstring             fileName;
    WSAEvents                *events;

    FileUploadStruct(ConnectionConfigurations *connConfig, std::queue<std::string> *outputBuffer
                     , std::wstring fileName, WSAEvents *events)
        : connConfig(connConfig)
        , outputBuffer(outputBuffer)
        , fileName(fileName)
        , events(events) {}
};
