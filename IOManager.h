#pragma once

#include <windows.h>
#include <winsock2.h>
#include "RoutineStructures.h"
#include "WSAEvents.h"

#define BUFFER_SIZE    65000

/*--------------- Globals --------------*/
typedef struct _SOCKET_INFORMATION
{
    OVERLAPPED Overlapped;
    SOCKET     ListenSocket, AcceptSocket;
    CHAR       Buffer[BUFFER_SIZE];
    WSABUF     DataBuf;
    DWORD      BytesSEND;
    DWORD      BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

class IOManager
{
public:

    IOManager();
    DWORD handleRead(LPVOID input);
    DWORD handleWrite(LPVOID input);
    DWORD handleFileRead(LPVOID input);
    DWORD handleConnect(WSAEvents *input);
    DWORD handleAccept(AcceptStruct *input);

    void CALLBACK readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void CALLBACK writeRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void CALLBACK fileReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void CALLBACK connectRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
};
