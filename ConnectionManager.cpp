#include "ConnectionManager.h"
#include <QDebug>
#include <QString>

ConnectionManager::ConnectionManager()
{
}


void ConnectionManager::createConnection(ConnectionConfigurations *connConfig)
{
    qDebug("%s", connConfig->ip->c_str());
    qDebug("Create connection");
}
