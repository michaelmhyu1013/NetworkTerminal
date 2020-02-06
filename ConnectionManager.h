#pragma once

#include "Connection.h"
#include "ConnectionConfigurations.h"

class ConnectionManager
{
public:
    ConnectionManager();
    void createConnection(ConnectionConfigurations *connConfig);

private:
    Connection *connection;
};
