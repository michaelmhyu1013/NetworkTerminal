#include "Client.h"


class TCPClient : public Client
{
public:
    TCPClient();
    TCPClient(SendStruct *ss)
        : Client(ss) {}

    int startup() override;
    int closeHandles() override;
protected:
    DWORD connectThreadID;
};
