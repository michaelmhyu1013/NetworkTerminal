#pragma once
#include <windows.h>
#include <winsock2.h>

/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		WSAEvents.h-	A struct that holds all Events within the system. Several handles are created
 *                                                              for each logical grouping of events that are related in signalling.
 * --
 * -- PROGRAM:			NetworkTerminal
 * --
 * -- DATE:             Feb 05, 2020
 * --
 * -- REVISIONS:
 * --		NA
 * --
 * -- DESIGNER:         Michael Yu
 * --
 * -- PROGRAMMER:		Michael Yu
 * --
 * -- NOTES:
 * -- This class holds all the events that are possible within the system that are used to signal completion routines
 * -- for overlapped I/O synchronization.
 * ----------------------------------------------------------------------------------------------------------------------*/

struct WSAEvents
{
    // FILE I/O STATES
    WSAEVENT DETECT_FILE_INPUT   = WSACreateEvent();
    WSAEVENT COMPLETE_FILE_INPUT = WSACreateEvent();

    // READING STATES
    WSAEVENT COMPLETE_READ = WSACreateEvent();

    // WRITING STATES
    WSAEVENT COMPLETE_WRITE = WSACreateEvent();

    // TCP CONNECTION STATES
    WSAEVENT DETECT_CONNECTION = WSACreateEvent();

    WSAEvents() {}
};
