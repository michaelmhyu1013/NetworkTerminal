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
#include <cmath>
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
    std::string  strBuffer{ buffer };
    qDebug("Size of write transfer is: %d characters", strBuffer.length());
    outputFile.open("output.txt", std::fstream::app);
    outputFile << strBuffer;
    outputFile.close();
}


DWORD IOManager::handleSend(SendStruct *input)
{
    DWORD      Flags{ 0 };
    OVERLAPPED overlapped;
    WSAEVENT   EventArray[1];
    DWORD      Index;
    WSABUF     sendBuf;
    int        n;

    EventArray[0] = input->events->COMPLETE_READ;

    qDebug("Waiting for file input to be detected and finish reading...");
    Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, FALSE);

    if ((Index - WSA_WAIT_EVENT_0) == 0)
    {
        qDebug("Read Complete. Perform sendRoutine");
    }

    if (!WSAResetEvent(EventArray[0]))
    {
        qDebug("Failed to reset event in handleSend");
    }
    DWORD totalBytes         = input->connConfig->totalBytes;
    int   totalTransmissions = input->connConfig->transmissions;
    int   packetSize         = input->connConfig->packetSize;
    int   packetsToSend      = std::ceil(totalBytes / static_cast<double>(packetSize));
    int   offset             = 0;

    sendBuf.len = packetSize;  // len needs to change depending on user selected packet size...
    char send[packetSize];
    sendBuf.buf = send;
    // send same buffer for now... TODO: parse through full file and needs to be in loop w/ offset
    memcpy(sendBuf.buf, &(input->outputBuffer)[0], packetSize);

    // tranmission times
    for (int i = 0; i < totalTransmissions; i++)
    {
        //packetize
        for (int j = 0; j < packetsToSend; j++)
        {
            if (input->connConfig->connectionType == 1)                 // TCP
            {
                if ((n = sendTCPPacket(input->clientSocketDescriptor, &sendBuf, 1, Flags,
                                       &overlapped, offset, packetSize)) < 0)
                {
                    qDebug("Sending TCP packet failed with error %d: ", n);
                }
            }
            else if (input->connConfig->connectionType == 0) //UDP
            {
                if ((n = sendUDPPacket(input->clientSocketDescriptor, &sendBuf, 1, Flags,
                                       &overlapped, offset, packetSize)) < 0)
                {
                    qDebug("Sending UDP packet failed with error %d: ", n);
                }
            }
        }

        offset = 0;
    }
} // IOManager::handleSend


DWORD IOManager::handleFileRead(FileUploadStruct *input)
{
    qDebug("handle File Read invoked");
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
        FILE_ATTRIBUTE_NORMAL,
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
            qDebug("ReadFile exited normally");
            break;
        }
        totalBytesRead += dwBytesRead;
    }
    CloseHandle(hFile);

    input->connConfig->totalBytes = totalBytesRead;

    if (!WSASetEvent(input->events->COMPLETE_READ))
    {
        qDebug("Failure to set COMPLETE_READ event: %d", WSAGetLastError());
        return(-2);
    }
    return(0);
} // IOManager::handleFileRead


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

    if ((n = ReadFileEx(hFile, input->outputBuffer, MAX_FILE_SIZE, &overlapped, sendRoutineEX)) == 0)
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
        // create thread for reading

        if (!WSAResetEvent(EventArray[0]))
        {
            qDebug("Failed to reset event in handleConnect");
        }

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
        SocketInfo->DataBuf.len = MAX_FILE_SIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;

        Flags = 0;

        qDebug("Socket %d connected\n", input->acceptSocketDescriptor);

        // TODO: check if RecvBytes should be set to NULL
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                    &(SocketInfo->Overlapped), readRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("WSARecv() failed with error: %d", WSAGetLastError());
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
//    HANDLE               writeThread;
//    DWORD                writeThreadID;
    DWORD RecvBytes;
    DWORD Flags;

    Flags = 0;
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
    SI->BytesRECV += BytesTransferred;

    // Too slow when threaded... directly append to file
    std::fstream outputFile;
    qDebug("Bytes received: %lu", SI->BytesRECV);
    std::string  strBuffer{ SI->DataBuf.buf };
    outputFile.open("output.txt", std::fstream::app);
    outputFile << strBuffer;
    qDebug("Size of write transfer is: %d characters", strBuffer.length());
    outputFile.close();
//    if ((writeThread = CreateThread(NULL, 0, onWriteToFile,
//                                    (LPVOID)SI->DataBuf.buf, 0, &writeThreadID)) == NULL)
//    {
//        qDebug("writeThread creation failed with error");
//    }

    SI->DataBuf.len = MAX_FILE_SIZE;
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
} // IOManager::readRoutine


void IOManager::sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
} // IOManager::sendRoutine


void IOManager::sendRoutineEX(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped)
{
    qDebug("sendRoutineEx");
    // just loop over buffer and send over stream.
}


/* -------------------------- HELPER FUNCS ------------------------ */
int IOManager::sendTCPPacket(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, int &offset, int packetSize)
{
    if (WSASend(s, lpBuffers, dwBufferCount, NULL, dwFlags,
                lpOverlapped, NULL) == 0)
    {
        qDebug("Sent bytes");
        offset += packetSize;
        return(0);
    }
    else
    {
        qDebug("Failed to send TCP Packet");
        return(-1);
    }
}


int IOManager::sendUDPPacket(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, int &offset, int packetSize)
{
    if (WSASendTo(s, lpBuffers, dwBufferCount, NULL, dwFlags, NULL, NULL,
                  lpOverlapped, NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            qDebug("WSASend() failed with error");
            return(-1);
        }
        else if (WSAGetLastError() == WSA_IO_PENDING)
        {
            qDebug("Sent bytes");
            offset += packetSize;
            return(0);
        }
    }
}
