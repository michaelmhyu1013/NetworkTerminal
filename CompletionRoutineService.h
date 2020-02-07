#pragma once

#include "RoutineStructures.h"
#include "IOManager.h"
#include "WSAEvents.h"
#include <windows.h>


struct CompletionRoutineService
{
    static DWORD WINAPI onReadRoutine(LPVOID param)
    {
        return(((IOManager *)param)->handleRead(nullptr));
    }


    static DWORD WINAPI onWriteRoutine(LPVOID param)
    {
        return(((IOManager *)param)->handleWrite(nullptr));
    }


    static DWORD WINAPI onFileUpload(LPVOID param)
    {
        return(((IOManager *)param)->handleFileRead(nullptr));
    }


    static DWORD WINAPI onConnectRoutine(LPVOID param)
    {
        return(((IOManager *)param)->handleConnect((WSAEvents *)param));
    }


    static DWORD WINAPI onAcceptRoutine(LPVOID param)
    {
        return(((IOManager *)param)->handleAccept((AcceptStruct *)param));
    }
};
