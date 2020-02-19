#pragma once

#include "ConnectionConfigurations.h"
#include "Definitions.h"
#include "GlobalUIManager.h"
#include "RoutineStructures.h"
#include "WSAEvents.h"
#include <TimeClock.h>
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
    DWORD         packetsReceived;
    DWORD         packetsToSend;
    WSAEVENT      completeReadEvent;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;


class IOManager
{
public:
    IOManager() {}
    DWORD handleSend(SendStruct *input);
    DWORD handleFileRead(FileUploadStruct *input);
    DWORD handleFileReadEX(FileUploadStruct *input);
    DWORD handleTCPClientConnect(AcceptStruct *input);
    DWORD handleAccept(AcceptStruct *input);
    DWORD handleUDPRead(AcceptStruct *input);
    DWORD performTCPConnect(SendStruct *input);
    DWORD handleWriteToScreen(AcceptStruct *input);

    void static CALLBACK readRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void static CALLBACK sendRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    void static CALLBACK UDPReadRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags);


    TimeClock       &clock = TimeClock::getInstance();
    GlobalUIManager &ui    = GlobalUIManager::getInstance();
    static DWORD    bytesTransferred; // for printing to terminal
    static int      packetsReceived;  // for printing to terminal

    ~IOManager();

private:
    int sendTCPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, int &offset, int packetSize);
    int sendUDPPacket(SOCKET s, WSABUF *lpBuffers, DWORD dwBufferCount, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, struct sockaddr *server, int size, int &offset, int packetSize);
    void static writeToFile(LPWSAOVERLAPPED Overlapped, DWORD BytesTransferred);
};
