#pragma once
#include <QString>
#include <stdio.h>
#include <string>
#include <WinSock2.h>

using namespace std;

class WindowsSocketManager
{
public:
   WindowsSocketManager();
   vector<QString> resolveHostNameToIP(const char *hostName);
   QString resolveIPToHostName(const char *ip);
   QString resolveServiceNameToPort(const char *serviceName, const char *service);
   QString resolvePortToServiceName(const char *port, const char *protocol);
};
