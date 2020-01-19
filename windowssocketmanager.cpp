/*---------------------------------------------------------------------------------------
--	SOURCE FILE:	windowssocketmanager.cpp -  A module that handles all API call related to
--                  the Windows Socket Application API.
--
--	PROGRAM:		resolutionAssignment1.exe
--
--	FUNCTIONS:		Winsock 2 API
--
--	DATE:			Jan 12, 2020
--
--	REVISIONS:		N/A
--
--	DESIGNERS:		Michael Yu
--
--	PROGRAMMER:		Michael Yu
--
--	NOTES:          Use this class to handle any network resolution requests that the user
--                  will request. All conversion is returned directly to the application and
--                  no previous storage of past entries will be stored.
--
---------------------------------------------------------------------------------------*/

#include "windowssocketmanager.h"
#include <QString>

using namespace std;
WindowsSocketManager::WindowsSocketManager()
{
}
QString WindowsSocketManager::resolveHostNameToIP(const char* hostName)
{
    QString         ip;
    int             a;
    struct          hostent *hp;
    struct          in_addr my_addr, *addr_p;
    char            **p;
    char            ip_address[256];      // String for IP address
    WSADATA         wsaData;

    WORD            wVersionRequested = MAKEWORD(2,2);
    WSAStartup(wVersionRequested, &wsaData);
    addr_p = (struct in_addr*)malloc(sizeof(struct in_addr));
    addr_p = &my_addr;
    addr_p->s_addr=inet_addr(ip_address) ;

    if ((hp = gethostbyname (hostName)) == NULL) {
        switch (h_errno) {
            case HOST_NOT_FOUND:
                return QString::fromStdString("No such host.");
            case TRY_AGAIN:
                return QString::fromStdString("Try again later.");
            case NO_RECOVERY:
                return QString::fromStdString("DNS Error.");
            case NO_ADDRESS:
                return QString::fromStdString("No IP Address.");
            default:
                return QString::fromStdString("Unknown Error");
        }

    }
    return ip;
}





QString WindowsSocketManager::resolveIPToHostName(const char* ip)
{
    QString         hostName;
    int             a;
    struct          hostent *hp;
    struct          in_addr my_addr, *addr_p;
    char            **p;
    char            ip_address[256];      // String for IP address
    WSADATA         wsaData;

    WORD            wVersionRequested = MAKEWORD(2,2);
    WSAStartup(wVersionRequested, &wsaData);
    addr_p = (struct in_addr*)malloc(sizeof(struct in_addr));
    addr_p = &my_addr;

    if (a = inet_addr(ip) == 0) {
        hostName = "IP Address must be in the form of X.X.X.X.";
    } else {
        addr_p->s_addr=inet_addr(ip);

        if ((hp = gethostbyaddr((char *)addr_p, PF_INET, sizeof(my_addr))) == NULL) {
            hostName = "Host information cannot be found.";
        }
    }
    return hostName;
}

QString WindowsSocketManager::resolveServiceNameToPort(const char* serviceName, const char* protocol)
{
    QString                 port;
    struct servent          *sv;
    WSADATA                 wsaData;

    WORD wVersionRequested = MAKEWORD(2,2);
    WSAStartup(wVersionRequested, &wsaData);
    sv = getservbyname(serviceName, protocol);

    port = sv != NULL ? QString::number(sv->s_port) : QString::fromStdString("Service and protocol does not resolve to a valid port.");
    return port;
}

QString WindowsSocketManager::resolvePortToServiceName(const char* port, const char* protocol)
{
    QString             serviceName;
    struct servent      *sv;
    int                 s_port;
    WSADATA             wsaData;

    WORD wVersionRequested = MAKEWORD(2,2);
    WSAStartup(wVersionRequested, &wsaData);
    s_port = atoi(port);
    sv = getservbyport(htons(s_port), protocol);

    serviceName = sv != NULL ? QString::fromUtf8(sv->s_name, -1) : QString::fromStdString("Port and protocol does not resolve to a valid service.");
    return serviceName;
}
