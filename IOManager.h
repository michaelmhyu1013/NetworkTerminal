#pragma once

#include "ConnectionConfigurations.h"
#include "Definitions.h"
#include "RoutineStructures.h"
#include "WSAEvents.h"
#include <windows.h>
#include <winsock2.h>

/*--------------- Globals --------------*/
typedef struct _SOCKET_INFORMATION
{
    WSAOVERLAPPED Overlapped;
    SOCKET        Socket;
    CHAR          Buffer[MAX_FILE_SIZE]; // TODO - CHANGE TO MALLOC SIZE OF BUFF
    WSABUF        DataBuf;
    DWORD         BytesSEND;
    DWORD         BytesRECV;
    DWORD         totalTransmissions;
    DWORD         packetSize;
    DWORD         packetsToSend;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

class IOManager
{
public:
    IOManager();
    DWORD handleSend(SendStruct *input);
    DWORD handleFileRead(FileUploadStruct *input);
    DWORD handleFileReadEX(FileUploadStruct *input);
    DWORD handleConnect(AcceptStruct *input);
    DWORD handleAccept(AcceptStruct *input);
    static DWORD WINAPI onWriteToFile(LPVOID param);


    void static CALLBACK readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void static CALLBACK sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void static CALLBACK sendRoutineEX(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped);

    int sendTCPPacket(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, int &offset, int packetSize);
    int sendUDPPacket(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, int &offset, int packetSize);~IOManager();

private:
};
