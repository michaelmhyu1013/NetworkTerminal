#pragma once
#include <stdio.h>
#define MAX_DEBUG_LENGTH    1024


void printDebug(char *str, const char *format, ...)
{
    char buffer[MAX_DEBUG_LENGTH];

    sprintf_s(buffer, "check it out: %s\n", "I can inject things");
}
