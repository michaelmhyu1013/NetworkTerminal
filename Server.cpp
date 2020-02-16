#include "Server.h"

Server::Server()
{
}


int Server::closeSockets()
{
    int n;

    if ((n = closesocket(asInfo->listenSocketDescriptor)) == SOCKET_ERROR)
    {
        qDebug("Close listening socket fail: %d", WSAGetLastError());
        return(n);
    }

    if ((n = closesocket(asInfo->acceptSocketDescriptor)) == SOCKET_ERROR)
    {
        qDebug("Close accept socket fail: %d", WSAGetLastError());
        return(n);
    }
    return(n);
}
