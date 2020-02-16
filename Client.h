#pragma once
#include "WindowsThreadService.h"


class Client
{
public:
    Client();
    Client(SendStruct *ss)
        : ss(ss) {}

    int virtual startup() = 0;
    int virtual closeHandles() = 0;
    int virtual closeSockets();
    int virtual readFileToOutputBuffer(FileUploadStruct *fs);

protected:
    WindowsThreadService *threadService;
    HANDLE               writeThread, fileThread, connectThread;
    WSADATA              *wsaData;
    SendStruct           *ss;
    DWORD                writeThreadID, fileThreadID, connectThreadID;
};
