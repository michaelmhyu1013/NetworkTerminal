#include "UDPServer.h"


/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		UDPServer.cpp -	A concrete implementation of the Server class that creates a UDP server on the
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
 * -- Once the UDPServer is established, the socket will simply attempt to read any packets that are sent through the
 * -- bound socket. Any data that is read will be written to the file 'output.txt'
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
 * -- Creates the threads of the UDP Server. A thread is created for connecting to the desired UDP server, and a write thread for
 * -- sending data to the UDP server. The write thread is in a wait state and only executes when a file input is successfully.
 * ----------------------------------------------------------------------------------------------------------------------*/
int UDPServer::startup()
{
    if ((readThread = CreateThread(NULL, 0, threadService->onUDPReadRoutine,
                                   (LPVOID)asInfo, 0, &readThreadID)) == NULL)
    {
        qDebug("readThread failed with error");
        return(-1);
    }

    if ((terminalOutputThread = CreateThread(NULL, 0, threadService->onAcceptRoutine,
                                             (LPVOID)asInfo, 0, &terminalOutputThreadID)) == NULL)
    {
        OutputDebugStringA("terminalThread failed with error\n");
        return(-2);
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
 * -- Closes the threads created by the UDPServer class.
 * ----------------------------------------------------------------------------------------------------------------------*/
int UDPServer::closeHandles()
{
    DWORD error;

    return(TerminateThread(readThread, error));
}
