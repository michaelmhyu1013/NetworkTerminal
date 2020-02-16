#pragma once
#include "Client.h"


class UDPClient : public Client
{
public:
    UDPClient();
    UDPClient(SendStruct *ss)
        : Client(ss) {}

    int startup() override;
    int closeHandles() override;
};
