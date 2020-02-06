#pragma once
#include "IOManager.h"
#include <windows.h>

struct CompletionRoutineService
{
    static DWORD WINAPI readRoutine(LPVOID param)
    {
        return(((IOManager *)param)->handleRead(nullptr));
    }


    static DWORD WINAPI writeRoutine(LPVOID param)
    {
        return(((IOManager *)param)->handleWrite(nullptr));
    }


    static DWORD WINAPI onFileUpload(LPVOID param)
    {
        return(((IOManager *)param)->handleFileRead(nullptr));
    }
};
