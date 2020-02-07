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
#include <winsock2.h>

ConnectionManager::ConnectionManager()
    : routineService(new CompletionRoutineService())
    , events(new WSAEvents())
    , asInfo(new AcceptStruct())
{
    asInfo->events = events;
}


void ConnectionManager::createUDPClient(ConnectionConfigurations *connConfig)
{
    qDebug("udpclient");
    memset((char *)&server, 0, sizeof(server));

    if ((n = configureClientAddressStructures(connConfig)) < 0)
    {
        qDebug("Bind UDPClient failed");
    }
    else
    {
        qDebug("Bind UDPClient success");
    }
}


void ConnectionManager::createTCPClient(ConnectionConfigurations *connConfig)
{
    qDebug("tcpclient");
    memset((char *)&server, 0, sizeof(struct sockaddr_in));

    if ((n = configureClientAddressStructures(connConfig)) < 0)
    {
        qDebug("Bind TCPClient failed");
    }
    else
    {
        qDebug("Bind TCPClient success");
    }
}


void ConnectionManager::createUDPServer(ConnectionConfigurations *connConfig)
{
    qDebug("udpserver");

    memset((char *)&server, 0, sizeof(server));

    if ((n = bindServer(connConfig) < 0))
    {
        qDebug("Bind UDPServer failed");
    }
    else
    {
        qDebug("Bind UDPServer success");
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
    qDebug("tcpserver");

    memset((char *)&server, 0, sizeof(struct sockaddr_in));

    if ((n = bindServer(connConfig) < 0))
    {
        qDebug("Bind TCPServer failed");
    }

    if (listen(asInfo->listenSocketDescriptor, 5) == SOCKET_ERROR)
    {
        qDebug("listen() failed with error ");
    }

    if ((events->DETECT_CONNECTION = WSACreateEvent()) == 0)
    {
        qDebug("Set event");
    }

    //  CREATE THREAD FOR I/O
    if ((connectThread = CreateThread(NULL, 0, routineService->onConnectRoutine,
                                      (LPVOID)events, 0, &connectThreadID)) == NULL)
    {
        qDebug("readThread failed with error");
    }

    if ((acceptThread = CreateThread(NULL, 0, routineService->onAcceptRoutine,
                                     (LPVOID)asInfo, 0, &acceptThreadID)) == NULL)
    {
        qDebug("acceptThread failed with error");
    }
} // ConnectionManager::createTCPServer


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


int ConnectionManager::configureClientAddressStructures(ConnectionConfigurations *connConfig)
{
    WSAStartup(wVersionRequested, &WSAData);

    if ((asInfo->listenSocketDescriptor = WSASocket(PF_INET, connConfig->socketConnectionType, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
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
