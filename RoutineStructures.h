#pragma once

#include "WSAEvents.h"
#include <windows.h>

struct AcceptStruct
{
    SOCKET    acceptSocketDescriptor; // server's socket that handles newly created client connection
    SOCKET    listenSocketDescriptor; // server's socket for listening to incoming client connect
    SOCKET    clientSocketDescriptor; // client's socket to bind to host
    WSAEvents *events;
};
