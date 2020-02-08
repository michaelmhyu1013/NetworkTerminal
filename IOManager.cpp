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

/* -------------------------- THREAD FUNCTIONS ------------------------ */


DWORD IOManager::handleSend(LPVOID input)
{
}


DWORD IOManager::handleFileRead(LPVOID input)
{
}


DWORD IOManager::handleAccept(AcceptStruct *input)
{
    while (TRUE)
    {
        // TODO: implement as AcceptEx()
        if ((input->acceptSocketDescriptor = accept(input->listenSocketDescriptor, NULL, NULL)) == INVALID_SOCKET)
        {
            qDebug("Listen failed: %d", WSAGetLastError());
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


DWORD IOManager::handleConnect(AcceptStruct *input)
{
    DWORD                Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT             EventArray[1];
    DWORD                Index;
    DWORD                RecvBytes;

    EventArray[0] = input->events->DETECT_CONNECTION; // Dummy event to put thread into alertable state for completion routine

    while (TRUE)
    {
        // Wait for accept() to signal an event and also process WorkerRoutine() returns.

        while (TRUE)
        {
            qDebug("Waiting for connection");

            // Set last param to TRUE; automatically returns from this function when the completion routine completes
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

            if (Index == WSA_WAIT_FAILED)
            {
                qDebug("WSAWaitForMultipleEvents failed with error");
            }

            if ((Index - WSA_WAIT_EVENT_0) == 0)
            {
                // An accept() call event is ready - break the wait loop
                qDebug("CONNECTION RECEIVED");
                break;
            }
        }

        if (!WSAResetEvent(EventArray[0]))
        {
            qDebug("Failed to reset event in handleConnect");
        }
        // Create a socket information structure to associate with the accepted socket.

        if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
                                                            sizeof(SOCKET_INFORMATION))) == NULL)
        {
            qDebug("GlobalAlloc() failed with error");
        }
        // TODO: IO PORTION IS DONE HERE --------------------------

        // Fill in the details of our accepted socket.

        SocketInfo->Socket = input->acceptSocketDescriptor;
        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND   = 0;
        SocketInfo->BytesRECV   = 0;
        SocketInfo->DataBuf.len = BUFFER_SIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;

        Flags = 0;

        qDebug("Socket %d connected\n", input->acceptSocketDescriptor);

        // TODO: check if RecvBytes should be set to NULL
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                    &(SocketInfo->Overlapped), readRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("WSARecv() failed with error");
                return(FALSE);
            }
        }
        else
        {
            qDebug("Posting WSARecv");
        }
    }
} // IOManager::handleConnect


/* -------------------------- COMPLETION ROUTINES ------------------------ */


void IOManager::readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    DWORD SendBytes, RecvBytes;
    DWORD Flags;

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

    if (Error != 0)
    {
        qDebug("I/O operation failed with error %lu", Error);
    }

    if (BytesTransferred == 0)
    {
        qDebug("Closing socket: %d", SI->Socket);
    }

    if (Error != 0 || BytesTransferred == 0)
    {
        closesocket(SI->Socket);
        GlobalFree(SI);
        return;
    }
    // Check to see if the BytesRECV field equals zero. If this is so, then
    // this means a WSARecv call just completed so update the BytesRECV field
    // with the BytesTransferred value from the completed WSARecv() call.

    if (SI->BytesRECV == 0)
    {
        SI->BytesRECV = BytesTransferred;
        SI->BytesSEND = 0;
    }
    else
    {
        SI->BytesSEND += BytesTransferred;
    }

    if (SI->BytesRECV > SI->BytesSEND)
    {
        // Post another WSASend() request.
        // Since WSASend() is not gauranteed to send all of the bytes requested,
        // continue posting WSASend() calls until all received bytes are sent.

        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

        SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
        SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

        // TODO : WRITE TO FILE RATHER THAN SEND BACK TO CLIENT...

        if (WSASend(SI->Socket, &(SI->DataBuf), 1, NULL, 0,
                    &(SI->Overlapped), readRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("WSASend() failed with error %d", WSAGetLastError());
                return;
            }
        }
    }
    else
    {
        SI->BytesRECV = 0;

        // Now that there are no more bytes to send post another WSARecv() request.

        Flags = 0;
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

        SI->DataBuf.len = BUFFER_SIZE;
        SI->DataBuf.buf = SI->Buffer;

        if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
                    &(SI->Overlapped), readRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("WSARecv() failed with error %d", WSAGetLastError());
                return;
            }
        }
    }
} // IOManager::readRoutine


void IOManager::sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
} // IOManager::readRoutine
