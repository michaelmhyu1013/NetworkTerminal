#pragma once
#include <windows.h>

class IOManager
{
public:
    IOManager();
    DWORD handleRead(LPVOID input);
    DWORD handleWrite(LPVOID input);
    DWORD handleFileRead(LPVOID input);
};
