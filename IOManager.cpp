/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: IOManager.cpp - This class encapsulates all Windows Completion Routine functions
 * --               that can be called from our application. Key functionalities involve reading and
 * --               writing from a socket input, and reading a file input from the local disk.
 * --               The completion routine functions will be passed as the parameter for the
 * --               creation of a Windows thread to accomplish multi-processing.
 * --
 * --
 * -- PROGRAM: NetworkTerminal
 * --
 * -- FUNCTIONS:
 * --
 * --
 * -- DATE: Feb 5, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- NOTES:
 * --
 * ----------------------------------------------------------------------------------------------------------------------*/
#include "IOManager.h"
#include <QDebug>

IOManager::IOManager()
{
}


DWORD IOManager::handleRead(LPVOID input)
{
}


DWORD IOManager::handleWrite(LPVOID input)
{
}


DWORD IOManager::handleFileRead(LPVOID input)
{
}


DWORD IOManager::handleAccept(AcceptStruct *input)
{
    while (TRUE)
    {
        if ((input->acceptSocketDescriptor = accept(input->listenSocketDescriptor, NULL, NULL)) == INVALID_SOCKET)
        {
            int n = WSAGetLastError();
            qDebug("Listen failed");
        }

        if (WSASetEvent(input->events->DETECT_CONNECTION) == FALSE)
        {
            qDebug("WSASetEvent failed with error");
        }
        else
        {
            qDebug("Event set in handleAccept");
        }
    }
}


DWORD IOManager::handleConnect(WSAEvents *input)
{
    DWORD                Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT             EventArray[1];
    DWORD                Index;
    DWORD                RecvBytes;

    EventArray[0] = input->DETECT_CONNECTION;

    while (TRUE)
    {
        // Wait for accept() to signal an event and also process WorkerRoutine() returns.

        while (TRUE)
        {
            qDebug("Waiting for connection");

            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, FALSE);

            if (Index == WSA_WAIT_FAILED)
            {
                qDebug("WSAWaitForMultipleEvents failed with error");
            }

            if ((Index - WSA_WAIT_EVENT_0) == 0)
            {
                // An accept() call event is ready - break the wait loop
                qDebug("CONNECTION RECEIVED");
                WSAResetEvent(EventArray[0]);
                break;
            }
        }

        if (WSAResetEvent(EventArray[0]))
        {
            qDebug("Event reset in handleConnect");
        }
        // Create a socket information structure to associate with the accepted socket.

        if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
                                                            sizeof(SOCKET_INFORMATION))) == NULL)
        {
            qDebug("GlobalAlloc() failed with error");
        }
    }
} // IOManager::handleConnect
