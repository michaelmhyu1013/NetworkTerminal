#include "windowssocketmanager.h"

#pragma once

class DialogManager
{
public:
    DialogManager();
    WindowsSocketManager * getWinsockManager();

private:
    WindowsSocketManager *winsockManager;
};

