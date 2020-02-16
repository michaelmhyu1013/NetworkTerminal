#include "TCPServer.h"

TCPServer::TCPServer()
{
}


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


int TCPServer::closeHandles()
{
    return(CloseHandle(acceptThread) && CloseHandle(connectThread));
}
