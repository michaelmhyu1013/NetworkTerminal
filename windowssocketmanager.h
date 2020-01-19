#pragma once
#include <string>
#include <stdio.h>
#include <WinSock2.h>
#include <QString>

using namespace std;

class WindowsSocketManager
{
public:
    WindowsSocketManager();
    QString resolveHostNameToIP(const char* hostName);
    QString resolveIPToHostName(const char* ip);
    QString resolveServiceNameToPort(const char* serviceName, const char* service);
    QString resolvePortToServiceName(const char* port, const char* protocol);

};
