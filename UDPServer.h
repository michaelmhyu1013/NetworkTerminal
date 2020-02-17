#include "Server.h"

#pragma once
class UDPServer : public Server
{
public:
    UDPServer() {}
    UDPServer(AcceptStruct *asInfo)
        : Server(asInfo) {}

    int startup() override;
    int closeHandles() override;

protected:
    DWORD readThreadID;
};
