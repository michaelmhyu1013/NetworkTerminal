#include "dialogmanager.h"

DialogManager::DialogManager()
{
    winsockManager = new WindowsSocketManager();
}

WindowsSocketManager * DialogManager::getWinsockManager() {
    return this->winsockManager;
}
