#pragma once

#include "ConnectionConfigurations.h"
#include "Definitions.h"
#include "RoutineStructures.h"
#include "WSAEvents.h"
#include <Windows.h>
#include <WinSock2.h>

/*--------------- Globals --------------*/
typedef struct _SOCKET_INFORMATION
{
    WSAOVERLAPPED Overlapped;
    SOCKET        Socket;
    CHAR          Buffer[9600];
    WSABUF        DataBuf;
    DWORD         BytesSEND;
    DWORD         BytesRECV;
    int           TotalPackets;
    int           *PacketSize;
    DWORD         *TotalBytesReceived;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

class IOManager
{
public:
    IOManager();
    DWORD handleSend(SendStruct *input);
    DWORD handleFileRead(FileUploadStruct *input);
    DWORD handleConnect(AcceptStruct *input);
    DWORD handleAccept(AcceptStruct *input);
    DWORD handleUDPRead(UDPServerStruct *input);

    DWORD handleFileReadEX(FileUploadStruct *input);

    static DWORD WINAPI onWriteToFile(LPVOID param);

    void static CALLBACK UDPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void static CALLBACK TCPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void static CALLBACK sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void static CALLBACK sendRoutineEX(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
};
