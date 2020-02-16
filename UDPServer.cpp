#include "UDPServer.h"

UDPServer::UDPServer()
{
}


int UDPServer::startup()
{
    if ((readThread = CreateThread(NULL, 0, threadService->onUDPReadRoutine,
                                   (LPVOID)asInfo, 0, &readThreadID)) == NULL)
    {
        qDebug("readThread failed with error");
        return(-1);
    }
    return(0);
}


int UDPServer::closeHandles()
{
    return(CloseHandle(readThread));
}
