#pragma once

#include "ConnectionConfigurations.h"
#include "RoutineStructures.h"
#include "WSAEvents.h"
#include <winsock2.h>
#include <windows.h>

#define BUFFER_SIZE    65000

/*--------------- Globals --------------*/
typedef struct _SOCKET_INFORMATION
{
    WSAOVERLAPPED Overlapped;
    SOCKET        Socket;
    CHAR          Buffer[BUFFER_SIZE];
    WSABUF        DataBuf;
    DWORD         BytesSEND;
    DWORD         BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

class IOManager
{
public:
    IOManager();
    DWORD handleSend(SendStruct *input);
    DWORD handleFileRead(FileUploadStruct *input);
    DWORD handleConnect(AcceptStruct *input);
    DWORD handleAccept(AcceptStruct *input);

    void static CALLBACK readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void static CALLBACK sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
};
