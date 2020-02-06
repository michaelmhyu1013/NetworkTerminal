/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: dialogmanager.cpp - Provides the user with a graphical interface for resolving IP Address and
 * -- Host names. It processes user input by interfacing with the Windows Socket API, made available through
 * -- the WindowsSocketManager class.
 * --
 * -- PROGRAM: resolutionAssignment1
 * --
 * -- FUNCTIONS:
 * -- void configureHostToIP()
 * -- void configureIPToHost()
 * -- void on_pushButton_clicked()
 * --
 * -- DATE: January 12, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- NOTES:
 * -- This class handles all functionality related to dialog.ui, created using the Qt framework. Invalid
 * -- inputs will be inherently handled by the Windows Socket API error handling and displayed.
 * ----------------------------------------------------------------------------------------------------------------------*/
#include "dialogmanager.h"

DialogManager::DialogManager()
{
   winsockManager = new WindowsSocketManager();
}


WindowsSocketManager *DialogManager::getWinsockManager()
{
   return(this->winsockManager);
}
