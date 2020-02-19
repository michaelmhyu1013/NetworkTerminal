#include "TCPClient.h"

/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		TCPClient.cpp -	Represents an concrete implementation of a TCPClient that can establish a connection
 * --                                   to a TCP server on the designated port and host or ip address.
 * --
 * -- PROGRAM:			NetworkTerminal
 * --
 * -- FUNCTIONS:
 * --                   int startup()
 * --                   int closeHandles()
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
 * -- NOTES: This class creates the necessary threads that allow the client to write data to the TCP server. The packet
 * -- size is specified by the Connection Configuration structure that is passed during construction of the server.
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
int TCPClient::startup()
{
    if ((connectThread = CreateThread(NULL, 0, threadService->onAttemptClientConnection,
                                      (LPVOID)ss, 0, &connectThreadID)) == NULL)
    {
        OutputDebugStringA("connectThread failed with error\n");
        return(-2);
    }

    //  Create thread for sending; will WFMO for COMPLETE_READ event signaled by fileThread
    if ((writeThread = CreateThread(NULL, 0, threadService->onSendRoutine,
                                    (LPVOID)ss, 0, &writeThreadID)) == NULL)
    {
        OutputDebugStringA("writeThread failed with error\n");
        return(-1);
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
 * -- Closes the threads created by the TCPClient class.
 * ----------------------------------------------------------------------------------------------------------------------*/
int TCPClient::closeHandles()
{
    DWORD error;

    return(TerminateThread(writeThread, error) && TerminateThread(connectThread, error) && TerminateThread(fileThread, error));
}
