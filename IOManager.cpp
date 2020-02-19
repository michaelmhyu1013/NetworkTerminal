/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: IOManager.cpp - This class encapsulates all Windows Completion Routine functions nad thread
 * --               functions that can be called from our application. Key functionalities involve reading and
 * --               writing from a socket input, and reading a file input from the local disk.
 * --               The completion routine functions will be passed as the parameter for the
 * --               creation of a Windows thread to accomplish multi-processing.
 * --
 * --
 * -- PROGRAM: NetworkTerminal
 * --
 * -- FUNCTIONS:
 * --                   DWORD handleSend(SendStruct *input);
 * --                   DWORD handleFileRead(FileUploadStruct *input);
 * --                   DWORD handleFileReadEX(FileUploadStruct *input);
 * --                   DWORD handleTCPClientConnect(AcceptStruct *input);
 * --                   DWORD handleAccept(AcceptStruct *input);
 * --                   DWORD handleUDPRead(AcceptStruct *input);
 * --                   DWORD performTCPConnect(SendStruct *input);
 *
 * --                   void static CALLBACK readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
 * --                   void static CALLBACK sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
 * --                   void static CALLBACK UDPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
 *
 * --                   int sendTCPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, int &offset, int packetSize);
 * --                   int sendUDPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, struct sockaddr *server, int size, int &offset, int packetSize);
 * --                   void static writeToFile(LPWSAOVERLAPPED Overlapped, DWORD BytesTransferred);
 * --
 * -- DATE: Feb 5, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- NOTES:    The Server and Client classes creates threads that will depend on the callback funtions defined in this class.
 * --           This class is not instantiated but is referenced by WindowsTheadService in order to pass the static start routine
 * --           to the thread.
 * ----------------------------------------------------------------------------------------------------------------------*/
#include "IOManager.h"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mswsock.h>
#include <QDebug>


/* ---------------------------------------------------- THREAD FUNCTIONS ------------------------------------------------------ */
DWORD IOManager::handleFileRead(FileUploadStruct *input)
{
    qDebug("handle File Read invoked");
    HANDLE        hFile;
    DWORD         dwBytesRead, totalBytesRead{ 0 };
    const wchar_t *filename;
    int           n;

    filename = input->fileName.c_str();

    if ((hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
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


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: handleAccept
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: handleAccept(AcceptStruct *input)
 * --                       input   - structure that contains the socket that will be used to store the accepted client connection
 * --
 * -- RETURNS: 0 if a client connection is accepted, else a negative number
 * --
 * ----------------------------------------------------------------------------------------------------------------------*/
DWORD IOManager::handleAccept(AcceptStruct *input)
{
    while (input->isConnected)
    {
        while (input->isConnected)
        {
            // TODO: implement as AcceptEx()
            if ((input->acceptSocketDescriptor = accept(input->listenSocketDescriptor, NULL, NULL)) == INVALID_SOCKET)
            {
                qDebug("Listen failed: %d", WSAGetLastError());
                return(-1);
            }

            if (WSASetEvent(input->events->DETECT_CONNECTION) == FALSE)
            {
                qDebug("WSASetEvent failed with error");
                return(-2);
            }
            else
            {
                qDebug("Event set in handleAccept");
                return(0);
            }
        }
//        if (!AcceptEx(input->listenSocketDescriptor, input->acceptSocketDescriptor, static_cast<PVOID>(AcceptBuffer),
//                      0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, &listenOverlapped))
//        {
//            if (WSAGetLastError() != ERROR_IO_PENDING)
//            {
//                qDebug("AcceptEx() failed with error %d\n", WSAGetLastError());
//            }
//            else
//            {
//                qDebug("AcceptEx() is OK!\n");
//            }
//        }
    }
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: handleTCPClientConnect
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: handleTCPClientConnect(AcceptStruct *input)
 * --                       input   - structure that contains the socket that contains the connected client information
 * --
 * -- RETURNS: 0 if a client connection is accepted, else a negative number
 * --
 * -- NOTES:    Thread function that the TCP server will execute to listen for incoming connections and to read data that is
 * --           streamed from the socket. A global structure is allocated for the socket descriptor containing the connected
 * --           client information, which will need to be deallocated upon closing of the connection. The thread running this
 * --           function is placed in an an alertable state prior to posting a completion routine to handle reading data from
 * --           the client socket.
 * ----------------------------------------------------------------------------------------------------------------------*/
DWORD IOManager::handleTCPClientConnect(AcceptStruct *input)
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
        SocketInfo->BytesSEND       = 0;
        SocketInfo->BytesRECV       = 0;
        SocketInfo->packetsReceived = 0;
        SocketInfo->DataBuf.len     = MAX_BUFFER_SIZE;
        SocketInfo->DataBuf.buf     = SocketInfo->Buffer;

        Flags = 0;

        ui.getInstance().printToTerminal("Socket " + std::to_string(input->acceptSocketDescriptor) + " connected");

        // TODO: check if RecvBytes should be set to NULL
        WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                &(SocketInfo->Overlapped), readRoutine);
    }
} // IOManager::handleConnect


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: handleUDPRead
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: handleUDPRead(AcceptStruct *input)
 * --                       input   - structure that contains the socket that contains the connected client information
 * --
 * -- RETURNS: 0 if a client connection is accepted, else a negative number
 * --
 * -- NOTES:    Thread function that the UDP server will execute to listen for incoming data from the bound socket. The loop exits
 * --           upon the user closing the connection from the QT UI. A global structure is allocated for the socket descriptor
 * --           containing the connected client information, which will need to be deallocated upon closing of the connection.
 * --           The thread running this function is placed in an an alertable state prior to posting a completion routine to handle
 * --           reading data from the client socket.
 * ----------------------------------------------------------------------------------------------------------------------*/
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
    SocketInfo->BytesSEND       = 0;
    SocketInfo->BytesRECV       = 0;
    SocketInfo->packetsReceived = 0;
    Flags                       = 0;
    SocketInfo->DataBuf.len     = MAX_FILE_SIZE;
    SocketInfo->DataBuf.buf     = SocketInfo->Buffer; // assign the wsabuf to the socket buffer

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


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: handleSend
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: handleSend(SendStruct *input)
 * --                       input   - structure that contains the socket that contains the connected client information
 * --
 * -- RETURNS: 0 upon successfully exiting the send loop
 * --
 * -- NOTES:    Thread function that the TCPClient and UDPClient will execute to await for a valid file input to read data from.
 * --           The data will then be sent through the socket to the server in the designated packet size over the connection.
 * --           It will simply loop a designated amount of times to transmit the data. A global structure is allocated for the socket descriptor
 * --           containing the connected client information, which will need to be deallocated upon closing of the connection.
 * --           The thread running this function is placed in an an alertable state prior to posting a completion routine to handle
 * --           reading data from the client socket.
 * ----------------------------------------------------------------------------------------------------------------------*/
DWORD IOManager::handleSend(SendStruct *input)
{
    DWORD                Flags{ 0 };
    DWORD                Index;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT             EventArray[1];
    int                  n, count = 0;
    int                  packetSize = input->connConfig->packetSize;

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


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: performTCPConnect
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: performTCPConnect(SendStruct *input)
 * --                       input   - structure that contains the socket that contains the connected client information
 * --                                   and the server structure to connect to
 * --
 * -- RETURNS: 0 if a the TCP client can successfully connect to the server, else -1
 * --
 * -- NOTES:    Thread function that the TCPClient and UDPClient will execute to await for a valid file input to read data from.
 * --           The data will then be sent through the socket to the server in the designated packet size over the connection.
 * --           It will simply loop a designated amount of times to transmit the data. A global structure is allocated for the socket descriptor
 * --           containing the connected client information, which will need to be deallocated upon closing of the connection.
 * --           The thread running this function is placed in an an alertable state prior to posting a completion routine to handle
 * --           reading data from the client socket.
 * ----------------------------------------------------------------------------------------------------------------------*/
DWORD IOManager::performTCPConnect(SendStruct *input)
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
}


/* ------------------------------------------------------ COMPLETION ROUTINES ------------------------------------------------------ */


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: readRoutine
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
 * --                       Error               - stores the error that occurs from the reading operation
 * --                       BytesTransferred    - stores the number of bytes read from the WSARecv operation
 * --                       Overlapped          - pointer to the overlapped structure that will be used as the reference
 * --                                               to cast to the LPSOCKET_INFORMATION
 * --                       InFlags             - flags for the WSARecv operation
 * -- RETURNS: void
 * --
 * -- NOTES:    Completion routine to be executed by the TCP server upon a call to WSARecv is made. This function checks
 * --           to see if bytes were transferred from the WSARecv operation before closing the client socket. Upon successful
 * --           read from the socket buffer, the data transferred will be written to 'output.txt'.
 * ----------------------------------------------------------------------------------------------------------------------*/
void IOManager::readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    DWORD                RecvBytes;
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

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

    if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &InFlags,
                &(SI->Overlapped), readRoutine) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            qDebug("WSARecv() failed with error %d", WSAGetLastError());
            return;
        }

        if (WSAGetLastError() == WSAECONNRESET || WSAGetLastError() == WSAEDISCON)
        {
            qDebug("Client closd socket. Closing client socket.");
        }

        if (Error != 0)
        {
            qDebug("I/O operation failed with error %lu", Error);
        }
    }
} // IOManager::readRoutine


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: UDPReadRoutine
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: UDPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
 * --                       Error               - stores the error that occurs from the reading operation
 * --                       BytesTransferred    - stores the number of bytes read from the WSARecv operation
 * --                       Overlapped          - pointer to the overlapped structure that will be used as the reference
 * --                                               to cast to the LPSOCKET_INFORMATION
 * --                       InFlags             - flags for the WSARecv operation
 * -- RETURNS: void
 * --
 * -- NOTES:    Completion routine to be executed by the UDP server upon a call to WSARecvFrom is made. The data read
 * --           read from the socket buffer is simply written to the file 'output.txt'.
 * ----------------------------------------------------------------------------------------------------------------------*/
void IOManager::UDPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

    SI->BytesRECV += BytesTransferred;

    writeToFile(Overlapped, BytesTransferred);
} // IOManager::UDPReadRoutine


/*------------------------------------------------------ HELPER FUNCS ------------------------------------------------------ */


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: sendTCPPacket
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: sendTCPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, int &offset, int packetSize)
 * --                       s               - bound client socket that will be used to send data through
 * --                       lpBuffers       - pointer to the buffer(s) containing the data to be sent over the connection
 * --                       dwBufferCount   - number of buffers pointed to by lpBuffers
 * --                       dwFlags         - flags for the WSASend operation
 * --                       lpOverlapped    - overlapped structure that will hold any overlapped I/O from the WSASend operation
 * --                       offset          - integer value denoting the position of the current position of the file being read from
 * --                       packetSize      - designated integer size of the packet to be sent
 * --
 * -- RETURNS: 0 upon successful send through the socket, else -1
 * --
 * ----------------------------------------------------------------------------------------------------------------------*/
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


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: sendUDPPacket
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: sendUDPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
 * --                         struct sockaddr *server, int size, int &offset, int packetSize)
 * --                       s               - bound client socket that will be used to send data through
 * --                       lpBuffers       - pointer to the buffer(s) containing the data to be sent over the connection
 * --                       dwBufferCount   - number of buffers pointed to by lpBuffers
 * --                       dwFlags         - flags for the WSASend operation
 * --                       lpOverlapped    - overlapped structure that will hold any overlapped I/O from the WSASend operation
 * --                       server          - pointer to the sockaddr structure containing the information of the server to send data to
 * --                       size            - size of the sockaddr structure
 * --                       offset          - integer value denoting the position of the current position of the file being read from
 * --                       packetSize      - designated integer size of the packet to be sent
 * --
 * -- RETURNS: 0 upon successful send through the socket, else -1
 * --
 * ----------------------------------------------------------------------------------------------------------------------*/
int IOManager::sendUDPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
                             struct sockaddr *server, int size, int &offset, int packetSize)
{
    int   n;
    DWORD BytesTransferred;

    n = WSASendTo(s, lpBuffers, 1, &BytesTransferred, dwFlags,
                  server, size, lpOverlapped, NULL);

    if (n == 0 || (n == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))
    {
        qDebug("Sent bytes: %lu", BytesTransferred);
        offset++;  // use to keep count of packets sent for now
        return(0);
    }
    else
    {
        qDebug("Failed to send UDP Packet: %d", WSAGetLastError());
        return(-1);
    }
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: writeToFile
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: writeToFile(LPWSAOVERLAPPED Overlapped, DWORD BytesTransferred)
 * --                       Overlapped          - pointer to the overlapped structure that will be cast to a LPSOCKET_INFORMATION
 * --                                              to access the buffer storing the data to write
 * --                       BytesTransferred    - DWORD containing the number of bytes received from the WSARecvFrom operation
 * --
 * -- RETURNS: 0 upon successful send through the socket, else -1
 * --
 * ----------------------------------------------------------------------------------------------------------------------*/
void IOManager::writeToFile(LPWSAOVERLAPPED Overlapped, DWORD BytesTransferred)
{
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

    //    qDebug("Packets Received: %lu", ++(SI->BytesSEND));
    std::ofstream logFile{ "log.txt", std::ios_base::binary | std::fstream::app };
    std::fstream  outputFile{ "output.txt", std::ios_base::binary | std::fstream::app };

    logFile << "Packets Received: " << ++(SI->packetsReceived) << std::endl;
    logFile << "Size of Packet Received: " << BytesTransferred << " Bytes" << std::endl;
    logFile << "Total Received In Transfer: " << SI->BytesRECV << " Bytes" << std::endl << std::endl;

    outputFile.write(SI->DataBuf.buf, BytesTransferred);
    //    qDebug("Size of write transfer is: %d characters", strBuffer.length());
    outputFile.close();
}


void IOManager::writeStatisticsToLog(LPWSAOVERLAPPED Overlapped, DWORD BytesTransferred)
{
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;
}
