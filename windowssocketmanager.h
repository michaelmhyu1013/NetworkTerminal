#pragma once
#include <string>
using namespace std;

class WindowsSocketManager
{
public:
    WindowsSocketManager();
    string& resolveHostNameToIP(const string& hostName);
    string& resolveIPToHostName(const string& ip);
    string& resolveServiceNameToPort(const string& serviceName);
    string& resolvePortToServiceName(const string& port);
};
