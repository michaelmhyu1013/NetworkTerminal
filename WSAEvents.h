#pragma once
#include <WinSock2.h>
#include <Windows.h>


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

constexpr static int EVENT_COUNTS                  = 6;

struct WSAEvents
{
    // FILE I/O STATES
    WSAEVENT DETECT_FILE_INPUT   = WSACreateEvent();
    WSAEVENT COMPLETE_FILE_INPUT = WSACreateEvent();

    // READING STATES
    WSAEVENT DETECT_READ   = WSACreateEvent();
    WSAEVENT COMPLETE_READ = WSACreateEvent();

    // WRITING STATES
    WSAEVENT COMPLETE_WRITE = WSACreateEvent();

    // TCP CONNECTION STATES
    WSAEVENT DETECT_CONNECTION = WSACreateEvent();

    WSAEvents() {}
};
