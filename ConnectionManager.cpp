#include "ConnectionManager.h"
#include "TCPClient.h"
#include "TCPServer.h"
#include "UDPClient.h"
#include "UDPServer.h"
#include <QDebug>
#include <QString>
/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: ConnectionManager.cpp - Class that connects the QT gui to the underlying class structures for creating
 * --                                       a Client and Server. The Winsock DLL is initiated in this class to utilize
 * --                                       Berkeley sockets in our application. All binding of client and server address
 * --                                       structures are performed in this class prior to constructing the models.
 * --
 * --
 * -- PROGRAM: NetworkTerminal
 * --
 * -- FUNCTIONS:
 * --               void createUDPClient(ConnectionConfigurations *connConfig)
 * --               void createTCPClient(ConnectionConfigurations *connConfig)
 * --               void createUDPServer(ConnectionConfigurations *connConfig)
 * --               void createTCPServer(ConnectionConfigurations *connConfig)
 * --               void uploadFile(ConnectionConfigurations *connConfig, std::wstring fileName)
 * --               int bindServer(ConnectionConfigurations *connConfig)
 * --               int bindUDPClient(sockaddr_in &client, SendStruct *ss)
 * --               int configureClientAddressStructures(ConnectionConfigurations *connConfig, SendStruct *ss)
 * --               void cleanUp()
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
 * -- This class contains a single instance of a client and server sockaddr_in structure, and thus only one client connection
 * -- and server connection can be established at one instance in time.
 * ----------------------------------------------------------------------------------------------------------------------*/

ConnectionManager::ConnectionManager()
    : asInfo(new AcceptStruct())
    , events(new WSAEvents())
{
    asInfo->events = events;
    memset(outputBuffer, '\0', MAX_FILE_SIZE * sizeof(char));
}

/* ------------------------------------------------------ MAIN NETWORKING ------------------------------------------------------ */

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: createUDPClient
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void createUDPClient(ConnectionConfigurations *connConfig)
 * --                           connConfig - ConnectionConfigurations containing the user parameters that will be used to configure
 * --                                       the client sockaddr structure and will be passed to the UDPClient
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- All binding of necessary stuctures will be performed here and only upon success will the UDPClient be created.
 * ----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: createTCPClient
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void createTCPClient(ConnectionConfigurations *connConfig)
 * --                           connConfig - ConnectionConfigurations containing the user parameters that will be used to configure
 * --                                       the server sockaddr structure and will be passed to the TCPClient
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- All binding of necessary stuctures will be performed here and only upon success will the TCPClient be created.
 * ----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: createUDPServer
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void createTCPClient(ConnectionConfigurations *connConfig)
 * --                           connConfig - ConnectionConfigurations containing the user parameters that will be used to configure
 * --                                       the server sockaddr structure to accept data from the designated port and ip
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- All binding of necessary stuctures will be performed here and only upon success will the UDPServer be created.
 * ----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: createTCPServer
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void createTCPServer(ConnectionConfigurations *connConfig)
 * --                           connConfig - ConnectionConfigurations containing the user parameters that will be used to configure
 * --                                       the server sockaddr structure to accept data from the designated port and ip
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- All binding of necessary stuctures will be performed here and only upon success will the TCPServer be created.
 * ----------------------------------------------------------------------------------------------------------------------*/
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


/* ------------------------------------------------------ FILE PROCESSING ------------------------------------------------------ */

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: uploadFile
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void uploadFile(ConnectionConfigurations *connConfig, std::wstring fileName)
 * --                           connConfig - ConnectionConfigurations containing the user parameters that will be used to configure
 * --                                       the server sockaddr structure to accept data from the designated port and ip
 * --                           fileName - wstring containing the path to the file that will be read into the outputBuffer
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * ----------------------------------------------------------------------------------------------------------------------*/
void ConnectionManager::uploadFile(ConnectionConfigurations *connConfig, std::wstring fileName)
{
    // Create struct for buffer management and connection configurations
    FileUploadStruct *fs = new FileUploadStruct(fileName, connConfig, this->events, this->outputBuffer);

    clnt->readFileToOutputBuffer(fs);
}


/* ------------------------------------------------------ UTIL FUNCTIONS ------------------------------------------------------ */

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: bindServer
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: int bindServer(ConnectionConfigurations *connConfig)
 * --                           connConfig - ConnectionConfigurations containing the user parameters that will be used to configure
 * --                                       the server sockaddr structure to accept data from the designated port and ip
 * --
 * -- RETURNS: 0 on successful binding of server struct sockaddr, else a negative number
 * --
 * -- NOTES:
 * -- Binds the sockaddr structure to the ip and port designated by the ConnectionConfiguration structure that is passed from the
 * -- GUI interface. The socket is created in overlapped mode and will listen to connections from all incoming addresses.
 * ----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: bindUDPClient
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: int bindUDPClient(sockaddr_in &client, SendStruct *ss)
 * --                           ss      - SendStruct containing the socket that will be bound to the UDPClient to be used for data streaming
 * --                           client  - structure that holds the transport address and port for the client socket
 * --
 * -- RETURNS: 0 on successful binding of server struct sockaddr, else a negative number
 * --
 * -- NOTES:
 * -- Binds the sockaddr structure to the client socket.
 * ----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: configureClientAddressStructures
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: int configureClientAddressStructures(ConnectionConfigurations *connConfig, SendStruct *ss)
 * --                           connConfig - ConnectionConfigurations containing the user parameters that will be used to configure
 * --                                       the server sockaddr structure to accept data from the designated port and ip
 * --                           ss      - SendStruct containing the socket reference to store the Client socket
 * --
 * -- RETURNS: 0 if the client socket is successfully created and is connected to the designated host or IP address
 * --
 * -- NOTES:
 * -- This is where the application initializes the use of the Winsock DLL for the client.
 * ----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: cleanUp
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void cleanUp
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- Sets the connection boolean to false to break out of infinite loops within the threaded functions of the server and client.
 * -- The client or server sockets are closed prior to deallocating the client and server objects.
 * ----------------------------------------------------------------------------------------------------------------------*/
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
