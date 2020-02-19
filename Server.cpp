#include "Server.h"


/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		Server.cpp -	Represents an abstract implementation of a server that will be started on the designated
 * --                                   ip and port from the ConnectionConfiguration object.
 * --
 * -- PROGRAM:			NetworkTerminal
 * --
 * -- FUNCTIONS:
 * --                   int closeSockets()
 * --                   int readFileToOutputBuffer(FileUploadStruct *fs)
 * --
 * -- DATE:             Feb 14, 2020
 * --
 * -- REVISIONS:
 * --		NA
 * --
 * -- DESIGNER:         Michael Yu
 * --
 * -- PROGRAMMER:		Michael Yu
 * --
 * -- NOTES:
 * -- The threads and sockets within the Server will need to be appropriately closed in the TCPServer and UDPServer classes
 * -- that inherit from this class. Once created, the user will need to close the connection before attempting another
 * -- connection attempt.
 * ----------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: closeSockets
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: int closeSockets(void)
 * --
 * -- RETURNS: 0 on successful closing of the servers sockets, else a value of SOCKET_ERROR.
 * --
 * -- NOTES:
 * -- Call tihs function to close all connected sockets on the Server instance. To retrieve errors, call WSAGetLastError.
 * ----------------------------------------------------------------------------------------------------------------------*/
int Server::closeSockets()
{
    int n;

    shutdown(asInfo->acceptSocketDescriptor, SD_BOTH);
    qDebug("Shutdown code: %d", n);

    if ((n = closesocket(asInfo->listenSocketDescriptor)) == SOCKET_ERROR)
    {
        qDebug("Close listening socket fail: %d", WSAGetLastError());
        return(n);
    }

    if ((n = closesocket(asInfo->acceptSocketDescriptor)) == SOCKET_ERROR)
    {
        qDebug("Close accept socket fail: %d", WSAGetLastError());
        return(n);
    }
    return(n);
}
