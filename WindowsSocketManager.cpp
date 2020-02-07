/*---------------------------------------------------------------------------------------
 *  --	SOURCE FILE:	windowssocketmanager.cpp -  A module that handles all API call related to
 *  --                  the Windows Socket Application API. It returns the resolved value if the
 *  --                  parameters are valid, else a string containing the error will be returned.
 *  --
 *  --	PROGRAM:		resolutionAssignment1.exe
 *  --
 *  --	FUNCTIONS:		Winsock 2 API
 *  --
 *  --	DATE:			Jan 12, 2020
 *  --
 *  --	REVISIONS:		Feb 5, 2020
 * --                       Michael Yu
 * --                       - refactor resolveHostNameToIP and resolveIPToHostName to return strings
 *  --
 *  --	DESIGNERS:		Michael Yu
 *  --
 *  --	PROGRAMMER:		Michael Yu
 *  --
 *  --	NOTES:          Use this class to handle any network resolution requests that the user
 *  --                  will request. All conversion is returned directly to the application and
 *  --                  no previous storage of past entries will be stored.
 *  ---------------------------------------------------------------------------------------*/

#include "WindowsSocketManager.h"
#include <QString>

WindowsSocketManager::WindowsSocketManager()
{
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: resolveHostNameToIP
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: QString resolveHostNameToIP(const char* hostName)
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function receives two user inputs from the user for a host name. The host name is then
 * -- passed to the Windows Socket API to resolve the service name using the gethostbyname API call.
 * -- If there is valid entry, the resolved IP Address will be returned. Else, the appropriate error handler will
 * -- return a string depending on the type of error that occurred.
 * ----------------------------------------------------------------------------------------------------------------------*/
vector<std::string> WindowsSocketManager::resolveHostNameToIP(const char *hostName)
{
    vector<std::string>     values;
    QString                 ip;
    struct          hostent *hp { 0 };
    struct          in_addr my_addr, *addr_p, in;
    char                    **p;
    WSADATA                 wsaData;
    vector<std::string>     aliases;

    WORD                    wVersionRequested = MAKEWORD(2, 2);

    WSAStartup(wVersionRequested, &wsaData);
    addr_p = (struct in_addr *)malloc(sizeof(struct in_addr));
    addr_p = &my_addr;

    if ((hp = gethostbyname(hostName)) == NULL)
    {
        switch (h_errno)
        {
        case HOST_NOT_FOUND:
            ip = "No such host.";
            break;

        case TRY_AGAIN:
            ip = "Try again later.";
            break;

        case NO_RECOVERY:
            ip = "DNS Error.";
            break;

        case NO_ADDRESS:
            ip = "No IP Address.";
            break;

        default:
            ip = "Unknown Error";
        }
    }
    else
    {
        for (p = hp->h_addr_list; *p != 0; p++)
        {
            char **q;
            memcpy(&in.s_addr, *p, sizeof(in.s_addr));
            ip = inet_ntoa(in);

            values.push_back(ip.toStdString());

            for (q = hp->h_aliases; *q != 0; q++)
            {
                values.push_back(*q);
            }
        }
    }
    WSACleanup();
    return(values);
} // WindowsSocketManager::resolveHostNameToIP


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: resolveIPToHostName
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: QString resolveIPToHostName(const char* ip)
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function receives two user inputs from the user for an IP address. The IP Address is then
 * -- passed to the Windows Socket API to resolve the host name using the gethostbyaddr API call.
 * -- If there is valid entry, the resolved service name will be returned. Else, the appropriate error handler will
 * -- return a string depending on the type of error that occurred.
 * ----------------------------------------------------------------------------------------------------------------------*/
std::string WindowsSocketManager::resolveIPToHostName(const char *ip)
{
    std::string             hostName;
    int                     a;
    struct          hostent *hp { 0 };
    struct          in_addr my_addr, *addr_p;
    WSADATA                 wsaData;
    vector<std::string>     aliases;

    WORD                    wVersionRequested = MAKEWORD(2, 2);

    WSAStartup(wVersionRequested, &wsaData);
    addr_p = (struct in_addr *)malloc(sizeof(struct in_addr));
    addr_p = &my_addr;

    if ((a = inet_addr(ip)) == 0)
    {
        hostName = "IP Address must be in the form of X.X.X.X";
    }
    else
    {
        addr_p->s_addr = inet_addr(ip);
        hp             = gethostbyaddr(reinterpret_cast<char *>(addr_p), PF_INET, sizeof(my_addr));
        hostName       = (hp == NULL) ? "Host information cannot be found." : hp->h_name;
    }
    WSACleanup();
    return(hostName);
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: resolveServiceNameToPort
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: QString resolveServiceNameToPort(const char* serviceName, const char* protocol)
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function receives two user inputs from the user for a service name and protocol. These two parameters are then
 * -- passed to the Windows Socket API to resolve the port number using the getservbyname API call.
 * -- If there is valid entry, the resolved port number will be returned. Else, the appropriate error handler will
 * -- return a string depending on the type of error that occurred.
 * ----------------------------------------------------------------------------------------------------------------------*/
QString WindowsSocketManager::resolveServiceNameToPort(const char *serviceName, const char *protocol)
{
    QString        port;
    struct servent *sv { 0 };
    WSADATA        wsaData;

    WORD           wVersionRequested = MAKEWORD(2, 2);

    WSAStartup(wVersionRequested, &wsaData);
    sv = getservbyname(serviceName, protocol);

    port = sv != NULL ? QString::number(sv->s_port) : QString::fromStdString("Service and protocol does not resolve to a port.");
    WSACleanup();
    return(port);
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: resolvePortToServiceName
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: QString resolvePortToServiceName(const char* port, const char* protocol)
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function receives two user inputs from the user for a port and protocol. These two parameters are then
 * -- passed to the Windows Socket API to resolve the service name using the getservbyport API call.
 * -- If there is valid entry, the resolved service name will be returned. Else, the appropriate error handler will
 * -- return a string depending on the type of error that occurred.
 * ----------------------------------------------------------------------------------------------------------------------*/
QString WindowsSocketManager::resolvePortToServiceName(const char *port, const char *protocol)
{
    QString        serviceName;
    struct servent *sv { 0 };
    int            s_port;
    WSADATA        wsaData;

    WORD           wVersionRequested = MAKEWORD(2, 2);

    WSAStartup(wVersionRequested, &wsaData);
    s_port = atoi(port);
    sv     = getservbyport(htons(s_port), protocol);

    serviceName = sv != NULL ? QString::fromUtf8(sv->s_name, -1) : QString::fromStdString("Port and protocol does not resolve to a service.");
    WSACleanup();
    return(serviceName);
}
