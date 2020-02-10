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
#include <cstring>
#include <fstream>
#include <iostream>
#include <QDebug>

IOManager::IOManager()
{
}


/* -------------------------- THREAD FUNCTIONS ------------------------ */


DWORD WINAPI IOManager::onWriteToFile(LPVOID param)
{
    qDebug("Executing write to file thread");
    char         *buffer = static_cast<char *>(param);

    std::fstream outputFile;
//    std::string  strBuffer{ buffer };
    outputFile.open("output.txt", std::fstream::app);
    outputFile << buffer;
    outputFile.close();
}


DWORD IOManager::handleSend(SendStruct *input)
{
    DWORD                Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT             EventArray[1];
    DWORD                Index;

    EventArray[0] = input->events->COMPLETE_READ;

    qDebug("Waiting for file input to be detected and finish reading...");
    Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

    if ((Index - WSA_WAIT_EVENT_0) == 0)
    {
        qDebug("Read Complete. Perform sendRoutine");
    }

    if (!WSAResetEvent(EventArray[0]))
    {
        qDebug("Failed to reset event in handleSend");
    }

    if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
                                                        sizeof(SOCKET_INFORMATION))) == NULL)
    {
        qDebug("GlobalAlloc() failed with error");
    }
    else
    {
        qDebug("GlobalAlloc() completed");
    }
    SocketInfo->Socket = input->clientSocketDescriptor;

    ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
    SocketInfo->BytesSEND          = 0;
    SocketInfo->BytesRECV          = 0;
    SocketInfo->DataBuf.len        = input->connConfig->packetSize;
    SocketInfo->DataBuf.buf        = input->outputBuffer;
    SocketInfo->PacketSize         = &(input->connConfig->packetSize);
    SocketInfo->TotalBytesReceived = &(input->connConfig->totalTransmissionsize);
    SocketInfo->TotalPackets       = ceil(*(SocketInfo->TotalBytesReceived) / *(SocketInfo->PacketSize));
    Flags                          = 0;

    // TODO: check if RecvBytes should be set to NULL
    if (WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, NULL, Flags,
                &(SocketInfo->Overlapped), sendRoutine) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            qDebug("WSASend() failed with error");
            return(FALSE);
        }
        else
        {
            qDebug("Posting WSASend");
        }
    }
    else
    {
        qDebug("Posting WSASend");
    }
    WSAResetEvent(EventArray[0]);
} // IOManager::handleSend


DWORD IOManager::handleFileReadEX(FileUploadStruct *input)
{
    qDebug("handle File Read invoked");
    OVERLAPPED    overlapped{ 0 };
    HANDLE        hFile;
    DWORD         dwBytesRead, totalBytesRead{ 0 };
    const wchar_t *filename;
    int           n, Index;
    WSAEVENT      EventArray[1];

    EventArray[0] = input->events->COMPLETE_READ;
    filename      = input->fileName.c_str();

    hFile = CreateFile(
        filename,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
        );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        qDebug("Invalid file handle");
        return(-1);
    }
    PurgeComm(hFile, PURGE_RXCLEAR);

    if ((n = ReadFileEx(hFile, input->outputBuffer, MAX_FILE_SIZE, &overlapped, (LPOVERLAPPED_COMPLETION_ROUTINE)sendRoutineEX)) == 0)
    {
        qDebug("Readfileex failed with error %lu", GetLastError());
    }

    if (GetLastError() != ERROR_SUCCESS)
    {
        qDebug("GetLastError Readfileex error: %lu", GetLastError());
    }
    CloseHandle(hFile);

    Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);
    return(0);
} // IOManager::handleFileRead


DWORD IOManager::handleFileRead(FileUploadStruct *input)
{
    qDebug("handle File Read invoked");
    OVERLAPPED    overlapped{ 0 };
    HANDLE        hFile;
    DWORD         dwBytesRead, totalBytesRead{ 0 };
    const wchar_t *filename;
    int           n;

    filename = input->fileName.c_str();

    hFile = CreateFile(
        filename,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
        );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        qDebug("Invalid file handle");
        return(-1);
    }
    PurgeComm(hFile, PURGE_RXCLEAR);

    while ((n = ReadFile(hFile, input->outputBuffer, input->connConfig->packetSize, &dwBytesRead, NULL)))
    {
        if (dwBytesRead == 0)
        {
            qDebug("Total bytes read: %lu", totalBytesRead);

            if (n == 0)
            {
                qDebug("Error in readfile: %lu", GetLastError());
            }
            input->connConfig->totalTransmissionsize = totalBytesRead;
            qDebug("ReadFile exited normally");
            break;
        }
        totalBytesRead += dwBytesRead;
    }
    CloseHandle(hFile);

    if (!WSASetEvent(input->events->COMPLETE_READ))
    {
        qDebug("Failure to set COMPLETE_READ event: %d", WSAGetLastError());
        return(-2);
    }
    return(0);
} // IOManager::handleFileRead


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
                qDebug("WSAWaitForMultipleEvents failed with error: %d", WSAGetLastError());
            }

            if ((Index - WSA_WAIT_EVENT_0) == 0)
            {
                // An accept() call event is ready - break the wait loop
                qDebug("Connection received");
                break;
            }
        }

        if (!WSAResetEvent(EventArray[0]))
        {
            qDebug("Failed to reset event in handleConnect");
        }

        if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
                                                            sizeof(SOCKET_INFORMATION))) == NULL)
        {
            qDebug("GlobalAlloc() failed with error");
        }
        // Fill in the details of our accepted socket.

        SocketInfo->Socket       = input->acceptSocketDescriptor;
        SocketInfo->TotalPackets = 0;
        SocketInfo->BytesSEND    = 0;
        SocketInfo->BytesRECV    = 0;
        SocketInfo->DataBuf.len  = MAX_FILE_SIZE;
        SocketInfo->DataBuf.buf  = SocketInfo->Buffer;
        SocketInfo->PacketSize   = &(input->connConfig->packetSize);
        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));

        Flags = 0;

        qDebug("Socket %d connected\n", input->acceptSocketDescriptor);

        // TODO: check if RecvBytes should be set to NULL
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                    &(SocketInfo->Overlapped), TCPReadRoutine) == SOCKET_ERROR)
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


DWORD IOManager::handleUDPRead(UDPServerStruct *input)
{
    qDebug("handleUDPRead");
    DWORD                Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT             EventArray[1];
    DWORD                Index;
    DWORD                RecvBytes;
    int                  client_len, n, bufferSize;

    if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
                                                        sizeof(SOCKET_INFORMATION))) == NULL)
    {
        qDebug("GlobalAlloc() failed with error: %lu", GetLastError());
    }
    // Fill in the details of our accepted socket.
    //    char buf[bufferSize];

    SocketInfo->Socket = *(input->listenSocketDescriptor);

    ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
    SocketInfo->TotalPackets = 0;
    SocketInfo->BytesSEND    = 0;
    SocketInfo->BytesRECV    = 0;
    SocketInfo->DataBuf.len  = MAX_FILE_SIZE;         //TODO make cc config global or pass it in somehow
    SocketInfo->DataBuf.buf  = SocketInfo->Buffer;
    SocketInfo->PacketSize   = &(input->connConfig->packetSize);

    EventArray[0] = input->events->DETECT_READ;
    Flags         = 0;

    while (TRUE)
    {
        // TODO: check if RecvBytes should be set to NULL
        if ((n = WSARecvFrom(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags, NULL, NULL,
                             &(SocketInfo->Overlapped), UDPReadRoutine)) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("WSARecvFrom() failed with error: %d", WSAGetLastError());
                return(FALSE);
            }
            else if (WSAGetLastError() == WSA_IO_PENDING)
            {
                qDebug("Posting new cycle for WSARecvFrom");
            }
        }

        if ((Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE)) != WSA_WAIT_IO_COMPLETION)
        {
            qDebug("WSAWaitForMultipleEvents failed to set alert state with error: %d", WSAGetLastError());
        }
        WSAResetEvent(EventArray[0]);
    }
} // IOManager::handleUDPRead


/* -------------------------- COMPLETION ROUTINES ------------------------ */
void IOManager::TCPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    qDebug("Executing tcpreadroutine");

    DWORD WrittenBytes, RecvBytes;
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
    }
    // Post another WSASend() request. // post another write request...
    // Since WSASend() is not gauranteed to send all of the bytes requested,
    // continue posting WSASend() calls until all received bytes are sent.
    // continue posting write requests until all bytes are written to the file...

    ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

    // TODO : WRITE TO FILE RATHER THAN SEND BACK TO CLIENT...
    //  Create thread for file upload

    HANDLE writeThread;
    DWORD  writeThreadID;
    qDebug("Bytes transferred in wsarecv: %lu", BytesTransferred);

    if ((writeThread = CreateThread(NULL, 0, onWriteToFile,
                                    (LPVOID)SI->DataBuf.buf, 0, &writeThreadID)) == NULL)
    {
        qDebug("writeThread creation failed with error");
    }
    SI->BytesRECV = 0;

    // Now that there are no more bytes to send post another WSARecv() request.

    Flags = 0;

    SI->DataBuf.len = *(SI->PacketSize);
    SI->DataBuf.buf = SI->Buffer;

    if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
                &(SI->Overlapped), TCPReadRoutine) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            qDebug("WSARecv() failed with error %d", WSAGetLastError());
            return;
        }
    }
} // IOManager::readRoutine


void IOManager::UDPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    qDebug("UDPReadRoutine");
    DWORD                WrittenBytes, RecvBytes;
    DWORD                Flags;
    int                  n;
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
    }
    else
    {
        SI->BytesSEND += BytesTransferred; //BYTESSEND IS EQUIV TO BYTES TO WRITE....
    }

    if (SI->BytesRECV > SI->BytesSEND)
    {
        // Post another WSASend() request. // post another write request...
        // Since WSASend() is not gauranteed to send all of the bytes requested,
        // continue posting WSASend() calls until all received bytes are sent.
        // continue posting write requests until all bytes are written to the file...

        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
        SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
        SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

        HANDLE writeThread;
        DWORD  writeThreadID;

        if ((writeThread = CreateThread(NULL, 0, onWriteToFile,
                                        (LPVOID)SI->DataBuf.buf, 0, &writeThreadID)) == NULL)
        {
            qDebug("writeThread creation failed with error");
        }
    }
    else
    {
        SI->BytesRECV = 0;

        // Now that there are no more bytes to send post another WSARecv() request.

        Flags = 0;
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

        SI->DataBuf.len = *(SI->PacketSize);
        SI->DataBuf.buf = SI->Buffer;

        if ((n = WSARecvFrom(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags, NULL, NULL,
                             &(SI->Overlapped), UDPReadRoutine)) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("WSARecv() failed with error %d", WSAGetLastError());
                return;
            }
        }
    }
} // IOManager::UDPReadRoutine


void IOManager::sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    qDebug("Executing sendRoutine in tcp");
    // TODO: FINISH SENDROUTINE
    DWORD SentBytes{ 0 };
    DWORD Flags;
    int   n, packetsSent;

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

    if (Error != 0)
    {
        qDebug("I/O operation failed with error %lu\n", Error);
    }

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
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

//        SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
        SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

        if ((n = WSASend(SI->Socket, &(SI->DataBuf), 1, NULL, Flags,
                         &(SI->Overlapped), NULL)) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("Failed to post sendRoutine %lu\n", Error);
            }
        }
    }
    else
    {
        SI->BytesRECV = 0;

        // Now that there are no more bytes to send post another WSARecv() request.

        Flags = 0;
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

        SI->DataBuf.len = *(SI->PacketSize);
        SI->DataBuf.buf = SI->Buffer;

        if ((n = WSASend(SI->Socket, &(SI->DataBuf), 1, NULL, Flags,
                         &(SI->Overlapped), sendRoutine)) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("Failed to post sendRoutine %lu\n", Error);
            }
        }
    }
} // IOManager::sendRoutine


void IOManager::sendRoutineEX(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    qDebug("SendRoutineEX");
    // TODO: FINISH SENDROUTINE
    DWORD SendBytes, RecvBytes;
    DWORD Flags;
    int   n, packetsSent;

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

//    if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
//                                                sizeof(SOCKET_INFORMATION))) == NULL)
//    {
//        qDebug("GlobalAlloc() failed with error");
//    }
    SI->BytesRECV = 0;
    SI->BytesSEND = 0;

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
    }
    else
    {
        qDebug("bytes send %lu", SI->BytesSEND);
        SI->BytesSEND += BytesTransferred; //BYTESSEND IS EQUIV TO BYTES TO WRITE....
    }

    if (SI->BytesRECV > SI->BytesSEND)

    {
        // Post another WSASend() request.
        // Since WSASend() is not guaranteed to send all of the bytes requested,
        // continue posting WSASend() calls until all received bytes are sent
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
        SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
        SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

        if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0, &(SI->Overlapped), sendRoutineEX) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSASend() failed with error %d\n", WSAGetLastError());
                return;
            }
        }
        else
        {
            printf("WSASend() is OK!\n");
        }
    }
} // IOManager::sendRoutineEX
