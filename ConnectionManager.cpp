/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: ConnectionManager.cpp -
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

#include "ConnectionManager.h"
#include <QDebug>
#include <QString>

ConnectionManager::ConnectionManager()
    : threadService(new WindowsThreadService())
    , events(new WSAEvents())
    , asInfo(new AcceptStruct())
{
    asInfo->events = events;
    memset(outputBuffer, '\0', MAX_FILE_SIZE * sizeof(char));
}


void ConnectionManager::uploadFile(ConnectionConfigurations *connConfig, std::wstring fileName)
{
    // Create struct for buffer management and connection configurations
    FileUploadStruct *fs = new FileUploadStruct(fileName, connConfig, this->events, this->outputBuffer);

    //  Create thread for file upload
    if ((fileThread = CreateThread(NULL, 0, threadService->onFileUpload,
                                   (LPVOID)fs, 0, &fileThreadID)) == NULL)
    {
        OutputDebugStringA("fileThread failed with error\n");
    }
} // ConnectionManager::createTCPServer


/* ------------------------- NETWORKING --------------------------- */
void ConnectionManager::createUDPClient(ConnectionConfigurations *connConfig)
{
    memset((char *)&server, 0, sizeof(server));
    memset((char *)&client, 0, sizeof(client));

    SendStruct *ss = new SendStruct(connConfig, this->events, this->outputBuffer);

    if ((n = configureClientAddressStructures(connConfig, ss)) < 0)
    {
        qDebug("Configure UDPClient Struct failed: %d", n);
    }

    if ((n = bindUDPClient(client, ss)) < 0)
    {
        qDebug("Bind to UDP server failed: %d", n);
    }
}


void ConnectionManager::createTCPClient(ConnectionConfigurations *connConfig)
{
    qDebug("tcpclient");

    SendStruct *ss = new SendStruct(connConfig, this->events, this->outputBuffer);
    memset((char *)&server, 0, sizeof(struct sockaddr_in));

    if ((n = configureClientAddressStructures(connConfig, ss)) < 0)
    {
        OutputDebugStringA("Bind TCPClient failed\n");
    }
    else
    {
        OutputDebugStringA("Bind TCPClient success\n");
    }

    if (connect(ss->clientSocketDescriptor, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        OutputDebugStringA("Failed to connect to desired host\n");
        closesocket(ss->clientSocketDescriptor);
        WSACleanup();
    }

    //  Create thread for sending; will WFMO for COMPLETE_READ event signaled by fileThread
    if ((writeThread = CreateThread(NULL, 0, threadService->onSendRoutine,
                                    (LPVOID)ss, 0, &writeThreadID)) == NULL)
    {
        OutputDebugStringA("writeThread failed with error\n");
    }
}


void ConnectionManager::createUDPServer(ConnectionConfigurations *connConfig)
{
    qDebug("udpserver");
    memset((char *)&server, 0, sizeof(server));

    if ((n = bindServer(connConfig) < 0))
    {
        OutputDebugStringA("Bind UDPServer failed\n");
    }
    else
    {
        OutputDebugStringA("Bind UDPServer success\n");
    }
    // Create read thread
    // loop forever
    // completion routine to read from socket
    // after read is done, set event for finished reading
    // wait for 'finished reading' event
    // create thread
    // write to file
}


void ConnectionManager::createTCPServer(ConnectionConfigurations *connConfig)
{
    OutputDebugStringA("tcpserver");

    memset((char *)&server, 0, sizeof(struct sockaddr_in));

    if ((n = bindServer(connConfig) < 0))
    {
        OutputDebugStringA("Bind TCPServer failed\n");
    }

    if (listen(asInfo->listenSocketDescriptor, 5) == SOCKET_ERROR)
    {
        OutputDebugStringA("listen() failed with error\n");
    }

    if ((connectThread = CreateThread(NULL, 0, threadService->onConnectRoutine,
                                      (LPVOID)asInfo, 0, &connectThreadID)) == NULL)
    {
        OutputDebugStringA("readThread failed with error\n");
    }

    if ((acceptThread = CreateThread(NULL, 0, threadService->onAcceptRoutine,
                                     (LPVOID)asInfo, 0, &acceptThreadID)) == NULL)
    {
        OutputDebugStringA("acceptThread failed with error\n");
    }
}


int ConnectionManager::bindServer(ConnectionConfigurations *connConfig)
{
    WSAStartup(wVersionRequested, &WSAData);

    if ((asInfo->listenSocketDescriptor = WSASocket(PF_INET, connConfig->socketConnectionType, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        qDebug("Can't create socket");
        return(-1);
    }
    server.sin_family      = AF_INET;
    server.sin_port        = htons(connConfig->port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(asInfo->listenSocketDescriptor, (PSOCKADDR)&server, sizeof(server)) == SOCKET_ERROR)
    {
        qDebug("Can't bind name to socket");
        return(-2);
    }
    return(0);
}


int ConnectionManager::configureClientAddressStructures(ConnectionConfigurations *connConfig, SendStruct *ss)
{
    WSAStartup(wVersionRequested, &WSAData);

    if ((ss->clientSocketDescriptor = WSASocket(PF_INET, connConfig->socketConnectionType, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        qDebug("Can't create socket");
        return(-1);
    }
    server.sin_family = AF_INET;
    server.sin_port   = htons(connConfig->port);

    if ((hp = gethostbyname((connConfig->ip)->c_str())) == NULL)
    {
        fprintf(stderr, "Can't get server's IP address\n");
        qDebug("Can't get server's IP address\n");
        return(-1);
    }
    memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);
    return(0);
}


int ConnectionManager::bindUDPClient(sockaddr_in &client, SendStruct *ss)
{
    int client_len;

    client.sin_family      = AF_INET;
    client.sin_port        = htons(0); // bind to any available port
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client_len             = sizeof(client);

    if (bind(ss->clientSocketDescriptor, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        perror("Can't bind name to socket");
        return(-1);
    }

    if (getsockname(ss->clientSocketDescriptor, (struct sockaddr *)&client, &client_len) < 0)
    {
        perror("getsockname: \n");
        return(-2);
    }
    qDebug("Port assigned is %d\n", ntohs(client.sin_port));
    return(0);
}
