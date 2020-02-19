#pragma once

#include "WindowsThreadService.h"

class Server
{
public:
    Server() {}
    Server(AcceptStruct *asInfo)
        : asInfo(asInfo) {}

    int virtual startup() = 0;
    int virtual closeHandles() = 0;
    int virtual closeSockets();

protected:
    AcceptStruct         *asInfo;
    HANDLE               acceptThread, readThread, writeThread, connectThread, fileThread, terminalOutputThread;
    DWORD                terminalOutputThreadID;
    WindowsThreadService *threadService;
    WSADATA              *wsaData;
};
