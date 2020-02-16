#include "Client.h"

Client::Client()
{
}


int Client::closeSockets()
{
    return(closesocket(ss->clientSocketDescriptor) == 0 ? 0 : WSAGetLastError());
}


int Client::readFileToOutputBuffer(FileUploadStruct *fs)
{
    if ((fileThread = CreateThread(NULL, 0, threadService->onFileUpload,
                                   (LPVOID)fs, 0, &fileThreadID)) == NULL)
    {
        OutputDebugStringA("fileThread failed with error\n");
    }
}
