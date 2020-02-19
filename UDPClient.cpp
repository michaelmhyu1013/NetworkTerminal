#include "UDPClient.h"


/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		UDPClient.cpp -	Represents an concrete implementation of a TCPClient that can establish a connection
 * --                                   to a TCP server on the designated port and host or ip address.
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
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- Creates the threads of the UDP Client. A write thread is created for the Client to send packets in a designated size
 * -- to the UDP Server endpoint. The write thread is in a wait state and only executes when a file input is successfully.
 * ----------------------------------------------------------------------------------------------------------------------*/
int UDPClient::startup()
{
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
 * -- Closes the threads created by the UDPClient class.
 * ----------------------------------------------------------------------------------------------------------------------*/
int UDPClient::closeHandles()
{
    DWORD error;

    return(TerminateThread(writeThread, error) && TerminateThread(fileThread, error));
}
