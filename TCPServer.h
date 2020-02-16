#pragma once

#include "Server.h"

class TCPServer : public Server
{
public:
    TCPServer();
    TCPServer(AcceptStruct *asInfo)
        : Server(asInfo) {}

    int startup() override;
    int closeHandles() override;
protected:
    DWORD connectThreadID, acceptThreadID;
};
