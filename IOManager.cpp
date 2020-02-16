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
    return(0);
}


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
    const wchar_t *filename;
    int           n, Index;
    WSAEVENT      EventArray[1];

    EventArray[0] = input->events->COMPLETE_READ;
    filename      = input->fileName.c_str();

    // This should techncially work if I global alloc the socket here with a buffer large enough to store the file...

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
    while (input->isConnected)
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

    while (input->isConnected)
    {
        while (input->isConnected)
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
            closesocket(input->acceptSocketDescriptor);
            closesocket(input->listenSocketDescriptor);
            WSACleanup();
            return(-1);
        }
        SocketInfo->Socket = input->acceptSocketDescriptor;

        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND   = 0;
        SocketInfo->BytesRECV   = 0;
        SocketInfo->DataBuf.len = MAX_BUFFER_SIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;

        Flags = 0;

        ui.getInstance().printToTerminal("Socket " + std::to_string(input->acceptSocketDescriptor) + " connected");

        // TODO: check if RecvBytes should be set to NULL
        WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                &(SocketInfo->Overlapped), readRoutine);
    }
} // IOManager::handleConnect


DWORD IOManager::handleUDPRead(AcceptStruct *input)
{
    DWORD                Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT             EventArray[1];
    DWORD                Index;
    DWORD                RecvBytes;
    int                  packetSize = input->connConfig->packetSize;

    EventArray[0] = WSACreateEvent();

    if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
                                                        sizeof(SOCKET_INFORMATION))) == NULL)
    {
        qDebug("GlobalAlloc() failed with error");
        closesocket(input->acceptSocketDescriptor);
        closesocket(input->listenSocketDescriptor);
        WSACleanup();
        return(-1);
    }
    int recvBufSize = 6000000;
    int err         = setsockopt(input->listenSocketDescriptor, SOL_SOCKET, SO_RCVBUF,
                                 (const char *)&recvBufSize, sizeof(recvBufSize));

    SocketInfo->Socket = input->listenSocketDescriptor;

    ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
    SocketInfo->BytesSEND   = 0;
    SocketInfo->BytesRECV   = 0;
    SocketInfo->DataBuf.len = MAX_FILE_SIZE;
    SocketInfo->DataBuf.buf = SocketInfo->Buffer; // assign the wsabuf to the socket buffer

    Flags = 0;

    while (input->isConnected)
    {
        if (WSARecvFrom(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, NULL, &Flags, NULL, NULL,
                        &(SocketInfo->Overlapped), UDPReadRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                qDebug("WSARecvFrom failed with error: %d", WSAGetLastError());
                return(FALSE);
            }
        }
        Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

        if (Index == WSA_WAIT_FAILED)
        {
            qDebug("UDP WSAWaitForMultipleEvents failed with error: %d", WSAGetLastError());
        }
    }
} // IOManager::handleUDPRead


DWORD IOManager::handleTCPConnect(SendStruct *input)
{
    clock.getInstance().start();

    if (connect(input->clientSocketDescriptor, (struct sockaddr *)input->server, sizeof(*(input->server))) == SOCKET_ERROR)
    {
        ui.getInstance().printToTerminal("Failed to connect to desired host\n");
        closesocket(input->clientSocketDescriptor);
        WSACleanup();
        return(-1);
    }
    clock.getInstance().end();
    ui.getInstance().printToTerminal("Time for connection is: " + std::to_string(clock.getInstance().getRoundTripTime()) + "ms");
    return(0);
} // IOManager::handleUDPRead


DWORD IOManager::handleSend(SendStruct *input)
{
    DWORD                Flags{ 0 };
    DWORD                Index, BytesTransferred;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT             EventArray[1];
    int                  n, count = 0;

    EventArray[0] = input->events->COMPLETE_READ;

    while (input->isConnected)
    {
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

        if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
                                                            sizeof(SOCKET_INFORMATION))) == NULL)
        {
            qDebug("GlobalAlloc() failed with error");
            closesocket(input->clientSocketDescriptor);
            GlobalFree(SocketInfo);
            WSACleanup();
            return(-1);
        }
        SocketInfo->Socket = input->clientSocketDescriptor;
        int packetSize = input->connConfig->packetSize;

        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND          = 0;
        SocketInfo->BytesRECV          = 0;
        SocketInfo->totalTransmissions = input->connConfig->transmissions;
        SocketInfo->packetsToSend      = std::ceil(input->connConfig->totalBytes / static_cast<double>(SocketInfo->packetSize));
        SocketInfo->DataBuf.len        = packetSize;

        char bufferSend[packetSize];
        SocketInfo->DataBuf.buf = bufferSend;

        // send same buffer for now... TODO: parse through full file and needs to be in loop w/ offset
        memcpy(SocketInfo->DataBuf.buf, &(input->outputBuffer)[0], packetSize);

        int offset = 0;

        // tranmission times - only send packetSize * total transmissions rather than whole file * total transmissions
        for (DWORD i = 0; i < SocketInfo->totalTransmissions; i++, count++)
        {
            //packetize
//        for (DWORD j = 0; j < SocketInfo->packetsToSend; j++, count++)
//        {
            if (input->connConfig->connectionType == 1)                 // TCP
            {
                if ((n = sendTCPPacket(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, Flags,
                                       &(SocketInfo->Overlapped), offset, packetSize)) < 0)
                {
                    qDebug("Sending TCP packet failed with error %d: ", n);
                }
            }
            else if (input->connConfig->connectionType == 0) //UDP
            {
                struct sockaddr serv = *((struct sockaddr *)input->server);
                int             size = sizeof(serv);

                if ((n = sendUDPPacket(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, Flags,
                                       &(SocketInfo->Overlapped), (struct sockaddr *)&serv, size, offset, packetSize)) < 0)
                {
                    qDebug("Sending UDP packet failed with error %d: ", n);
                }
            }
//        }

//        offset = 0;
        }

        ui.getInstance().printToTerminal("Total packets sent: " + std::to_string(offset));
    }
    GlobalFree(SocketInfo);
    return(0);
} // IOManager::handleSend


/* -------------------------- COMPLETION ROUTINES ------------------------ */
void IOManager::readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
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

    writeToFile(Overlapped, BytesTransferred);

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


void IOManager::UDPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    writeToFile(Overlapped, BytesTransferred);
} // IOManager::UDPReadRoutine


void IOManager::sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
} // IOManager::sendRoutine


void IOManager::sendRoutineEX(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped)
{
    qDebug("sendRoutineEx");
    // just loop over buffer and send over stream.
}


/* -------------------------- HELPER FUNCS ------------------------ */
int IOManager::sendTCPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, int &offset, int packetSize)
{
    int n = WSASend(s, lpBuffers, dwBufferCount, NULL, dwFlags, lpOverlapped, NULL);

    // Succeeds if it returns 0, or the I/O routine is to be completed later.
    if (n == 0 || (n == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))
    {
        qDebug("Sent bytes: %lu", lpOverlapped->InternalHigh);
        offset++;  // use to keep count of packets sent for now
        return(0);
    }
    else
    {
        qDebug("Failed to send TCP Packet: %d", WSAGetLastError());
        return(-1);
    }
}


int IOManager::sendUDPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
                             struct sockaddr *server, int size, int &offset, int packetSize)
{
    int   n;
    DWORD BytesTransferred;

    n = WSASendTo(s, lpBuffers, 1, &BytesTransferred, dwFlags,
                  server, size, lpOverlapped, NULL);

    if (n == 0 || (n == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))
    {
        qDebug("Sent bytes: %lu", lpOverlapped->InternalHigh);
        offset++;  // use to keep count of packets sent for now
        return(0);
    }
    else
    {
        qDebug("Failed to send UDP Packet: %d", WSAGetLastError());
        return(-1);
    }
}


void IOManager::writeToFile(LPWSAOVERLAPPED Overlapped, DWORD BytesTransferred)
{
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

//    qDebug("Packets Received: %lu", ++(SI->BytesSEND));
    std::fstream outputFile;
//    qDebug("Bytes received: %lu", BytesTransferred);
    std::string  strBuffer(SI->DataBuf.buf, BytesTransferred);

    outputFile.open("output.txt", std::fstream::app);
    outputFile << strBuffer;
//    qDebug("Size of write transfer is: %d characters", strBuffer.length());
    outputFile.close();
}
