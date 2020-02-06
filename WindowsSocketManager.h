#pragma once
#include <QString>
#include <stdio.h>
#include <string>
#include <winsock2.h>

using namespace std;

class WindowsSocketManager
{
public:
    WindowsSocketManager();
    vector<std::string> resolveHostNameToIP(const char *hostName);
    std::string resolveIPToHostName(const char *ip);
    QString resolveServiceNameToPort(const char *serviceName, const char *service);
    QString resolvePortToServiceName(const char *port, const char *protocol);
};
