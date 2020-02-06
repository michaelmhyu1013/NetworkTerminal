#include "WindowsSocketManager.h"

#pragma once

class DialogManager
{
public:
    DialogManager();

    WindowsSocketManager *getWinsockManager();

private:
    WindowsSocketManager *winsockManager;
};
