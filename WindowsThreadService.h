#pragma once

/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		WindowsThreadService.h-	A struct that holds all thread functions required in our application.
 * --
 * -- PROGRAM:			NetworkTerminal
 * --
 * -- DATE:             Feb 05, 2020
 * --
 * -- REVISIONS:
 * --		NA
 * --
 * -- DESIGNER:         Michael Yu
 * --
 * -- PROGRAMMER:		Michael Yu
 * --
 * -- NOTES:
 * -- This class holds the static thread functions that will be used in our application for calls to CreateThread.
 * -- All functions within this class are static member functions which can be passed as function pointer to CreateThread.
 * -- The parameter that is passed into the following thread functions can be cast to the desired structure as needed as the
 * -- paramters for the executing thread function.
 * --
 * -- This is required for class encapsulation for thread functions in Windows
 * ----------------------------------------------------------------------------------------------------------------------*/
#include "IOManager.h"
#include "RoutineStructures.h"
#include "WSAEvents.h"
#include <Windows.h>


struct WindowsThreadService
{
    static DWORD WINAPI onFileUpload(LPVOID param)
    {
        return((static_cast<IOManager *>(param))->handleFileRead(static_cast<FileUploadStruct *>(param)));
    }


    static DWORD WINAPI onSendRoutine(LPVOID param)
    {
        return((static_cast<IOManager *>(param))->handleSend(static_cast<SendStruct *>(param)));
    }


    static DWORD WINAPI onConnectRoutine(LPVOID param)
    {
        return((static_cast<IOManager *>(param))->handleConnect(static_cast<AcceptStruct *>(param)));
    }


    static DWORD WINAPI onAcceptRoutine(LPVOID param)
    {
        return((static_cast<IOManager *>(param))->handleAccept(static_cast<AcceptStruct *>(param)));
    }


    static DWORD WINAPI onUDPListenRoutine(LPVOID param)
    {
        return((static_cast<IOManager *>(param))->handleUDPRead(static_cast<UDPServerStruct *>(param)));
    }
};
