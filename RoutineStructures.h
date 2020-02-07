#pragma once

#include "WSAEvents.h"
#include <windows.h>

struct AcceptStruct
{
    SOCKET    acceptSocketDescriptor;
    SOCKET    listenSocketDescriptor;
    WSAEvents *events;
};
