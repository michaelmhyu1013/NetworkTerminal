#include "Client.h"

/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		Client.cpp -	Represents an abstract implementation of a client that can establish a connection
 * --                                   to a server using Berkeley Sockets. The implementing classes will spawn threads required
 * --                                   to perform file reading and sending stream data over the socket.
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
 * -- The threads and sockets within the Client will need to be appropriately closed in the TCPClient and UDPClient classes
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
 * -- RETURNS: 0 on successful closing of the clients sockets, else a value of SOCKET_ERROR.
 * --
 * -- NOTES:
 * -- Call tihs function to close all connected sockets on the Client instance. To retrieve errors, call WSAGetLastError.
 * ----------------------------------------------------------------------------------------------------------------------*/
int Client::closeSockets()
{
    qDebug("Shutdown client socket with code %d", shutdown(ss->clientSocketDescriptor, SD_BOTH));
    return(closesocket(ss->clientSocketDescriptor) == 0 ? 0 : WSAGetLastError());
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: readFileToOutputBuffer
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: int readFileToOutputBuffer(FileUploadStruct *fs)
 * --                      fs - structure that represents the paramters to be passed to the CreateThread function
 * --
 * -- RETURNS: 0 on successful creation of the thread, else -1
 * --
 * -- NOTES:
 * -- This function creates the thread that will read the input from a designated file from the user.
 * ----------------------------------------------------------------------------------------------------------------------*/
int Client::readFileToOutputBuffer(FileUploadStruct *fs)
{
    if ((fileThread = CreateThread(NULL, 0, threadService->onFileUpload,
                                   (LPVOID)fs, 0, &fileThreadID)) == NULL)
    {
        OutputDebugStringA("fileThread failed with error\n");
        return(-1);
    }
    return(0);
}
