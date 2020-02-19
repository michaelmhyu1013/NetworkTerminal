#include "TCPServer.h"

/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		TCPServer.cpp -	A concrete implementation of the Server class that creates a TCP server on the
 * --                           desired IP and port as specified by the user.
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
 * -- The TCPServer can currently service a single TCP client to connect to the server. The server simply listens for client connections,
 * -- and upon successful binding, the client can send data through the socket to the TCPServer. The data within the
 * -- socket is processed using a completion routine and written to the designated file.
 * ----------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: startup
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: int startup(void)
 * --
 * -- RETURNS: 0 if all threads are successfully created, else a negative number
 * --
 * -- NOTES:
 * -- Creates the threads of the TCP Server. A thread is created for connecting to the desired TCP server, and a write thread for
 * -- sending data to the TCP server. The write thread is in a wait state and only executes when a file input is successfully.
 * ----------------------------------------------------------------------------------------------------------------------*/
int TCPServer::startup()
{
    if (listen(asInfo->listenSocketDescriptor, 5) == SOCKET_ERROR)
    {
        OutputDebugStringA("listen() failed with error\n");
        return(-1);
    }

    if ((connectThread = CreateThread(NULL, 0, threadService->onConnectRoutine,
                                      (LPVOID)asInfo, 0, &connectThreadID)) == NULL)
    {
        OutputDebugStringA("connectThread failed with error\n");
        return(-2);
    }

    if ((acceptThread = CreateThread(NULL, 0, threadService->onAcceptRoutine,
                                     (LPVOID)asInfo, 0, &acceptThreadID)) == NULL)
    {
        OutputDebugStringA("acceptThread failed with error\n");
        return(-3);
    }
    return(0);
}

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: closeHandles
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: int closeHandles(void)
 * --
 * -- RETURNS: 0 if all threads of the client is successfully closed, else a negative number
 * --
 * -- NOTES:
 * -- Closes the threads created by the TCPServer class.
 * ----------------------------------------------------------------------------------------------------------------------*/
int TCPServer::closeHandles()
{
    DWORD error;
    return(TerminateThread(acceptThread, error) && TerminateThread(connectThread, error));
}
