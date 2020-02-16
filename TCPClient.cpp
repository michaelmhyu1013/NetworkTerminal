#include "TCPClient.h"

TCPClient::TCPClient()
{
}


int TCPClient::startup()
{
    if ((connectThread = CreateThread(NULL, 0, threadService->onTCPConnect,
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


int TCPClient::closeHandles()
{
    return(CloseHandle(writeThread) && CloseHandle(connectThread) && CloseHandle(fileThread));
}

