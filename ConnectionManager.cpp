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
#include "TCPClient.h"
#include "TCPServer.h"
#include "UDPClient.h"
#include "UDPServer.h"
#include <QDebug>
#include <QString>

ConnectionManager::ConnectionManager()
    : asInfo(new AcceptStruct())
    , events(new WSAEvents())
{
    asInfo->events = events;
    memset(outputBuffer, '\0', MAX_FILE_SIZE * sizeof(char));
}


/* ------------------------- MAIN NETWORKING --------------------------- */
void ConnectionManager::createUDPClient(ConnectionConfigurations *connConfig)
{
    memset((char *)&server, 0, sizeof(server));
    memset((char *)&client, 0, sizeof(client));

    SendStruct *ss = new SendStruct(connConfig, this->events, this->outputBuffer, &server, &clientConnected);

    if ((n = configureClientAddressStructures(connConfig, ss)) < 0)
    {
        qDebug("Configure UDPClient Struct failed: %d", n);
    }

    if ((n = bindUDPClient(client, ss)) < 0)
    {
        qDebug("Bind to UDP server failed: %d", n);
    }
    clnt = new UDPClient(ss);

    if ((n = clnt->startup()) < 0)
    {
        qDebug("Client startup failed with error: %d", n);
        WSACleanup();
        cleanUp();
    }
    clientConnected = true;
}


void ConnectionManager::createTCPClient(ConnectionConfigurations *connConfig)
{
    qDebug("tcpclient");

    SendStruct *ss = new SendStruct(connConfig, this->events, this->outputBuffer, &server, &clientConnected);
    memset((char *)&server, 0, sizeof(struct sockaddr_in));

    if ((n = configureClientAddressStructures(connConfig, ss)) < 0)
    {
        OutputDebugStringA("Bind TCPClient failed\n");
    }
    else
    {
        OutputDebugStringA("Bind TCPClient success\n");
    }
    clnt = new TCPClient(ss);

    if ((n = clnt->startup()) < 0)
    {
        qDebug("Client startup failed with error: %d", n);
        WSACleanup();
        cleanUp();
    }
    clientConnected = true;
}


void ConnectionManager::createUDPServer(ConnectionConfigurations *connConfig)
{
    qDebug("udpserver");
    asInfo->isConnected = &serverConnected;

    memset((char *)&server, 0, sizeof(server));

    if ((n = bindServer(connConfig) < 0))
    {
        qDebug("Bind UDPServer failed");
    }
    else
    {
        qDebug("Bind UDPServer success");
    }
    serv = new UDPServer(asInfo);

    if ((n = serv->startup()) < 0)
    {
        qDebug("Server startup failed with error: %d", n);
        WSACleanup();
        cleanUp();
    }
    serverConnected = true;
}


void ConnectionManager::createTCPServer(ConnectionConfigurations *connConfig)
{
    qDebug("tcpserver");
    asInfo->isConnected = &serverConnected;

    memset((char *)&server, 0, sizeof(struct sockaddr_in));

    if ((n = bindServer(connConfig) < 0))
    {
        OutputDebugStringA("Bind TCPServer failed\n");
    }
    serv = new TCPServer(asInfo);

    if ((n = serv->startup()) < 0)
    {
        qDebug("Server startup failed with error: %d", n);
        WSACleanup();
        cleanUp();
    }
    serverConnected = true;
}


/* ------------------------- FILE PROCESSING - TODO: move to separate class --------------------------- */
void ConnectionManager::uploadFile(ConnectionConfigurations *connConfig, std::wstring fileName)
{
    // Create struct for buffer management and connection configurations
    FileUploadStruct *fs = new FileUploadStruct(fileName, connConfig, this->events, this->outputBuffer);

    clnt->readFileToOutputBuffer(fs);
}


/* -------------------------------- UTIL FUNCTIONS - TODO: move to separate class ------------------------------ */
int ConnectionManager::bindServer(ConnectionConfigurations *connConfig)
{
    WSAStartup(wVersionRequested, &WSAData);
    asInfo->connConfig = connConfig;

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
        qDebug("Can't bind name to socket: %d", WSAGetLastError());
        return(-2);
    }
    return(0);
}


int ConnectionManager::bindUDPClient(sockaddr_in &client, SendStruct *ss)
{
    int client_len;

    client.sin_family      = AF_INET;
    client.sin_port        = htons(0); // bind to any available port
    client.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(ss->clientSocketDescriptor, (struct sockaddr *)&client, sizeof(client)) == -1)
    {
        perror("Can't bind name to socket");
        return(-1);
    }
    client_len = sizeof(client);

    if (getsockname(ss->clientSocketDescriptor, (struct sockaddr *)&client, &client_len) < 0)
    {
        perror("getsockname: \n");
        return(-2);
    }
    qDebug("Port assigned is %d\n", ntohs(client.sin_port));
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


/*
 * Clean up all resources
 */
void ConnectionManager::cleanUp()
{
    if (clientConnected)
    {
        clientConnected = false;
        qDebug("Client closed sockets with return value: %d", clnt->closeSockets());
        qDebug("Client closed thread handles with return value: %d", clnt->closeHandles());
        free(clnt);
    }

    if (serverConnected)
    {
        serverConnected = false;
        qDebug("Server closed sockets with return value: %d", serv->closeSockets());
        qDebug("Server closed thread handles with return value: %d", serv->closeHandles());
        free(serv);
    }
}

ConnectionManager::~ConnectionManager()
{
    clnt->closeHandles();
    serv->closeHandles();
    free(outputBuffer);
    free(clnt);
    free(serv);
    WSACleanup();
}
