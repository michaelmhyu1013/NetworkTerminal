#include "UDPClient.h"

UDPClient::UDPClient()
{
}


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


int UDPClient::closeHandles()
{
    return(CloseHandle(writeThread) && CloseHandle(fileThread));
}
