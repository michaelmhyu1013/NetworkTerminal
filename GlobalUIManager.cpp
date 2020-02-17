#include <GlobalUIManager.h>
/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: GlobalUIManager.cpp - Static instance that gives the application access to the QT ui for printing
 * --                                    statistics and information to the application window.
 * --
 * --
 * -- PROGRAM: NetworkTerminal
 * --
 * -- FUNCTIONS:
 * --               void printToTerminal(std::string s)
 * --
 * -- DATE: Feb 12, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- NOTES:
 * -- A single static instance of this class is created in the application for the purposes of accessing the QT UI. Reference
 * -- the instance by using the getInstance() method of this class.
 * ----------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: printToTerminal
 * --
 * -- DATE: Feb14, 2020
 * --
 * -- REVISIONS:
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void printToTerminal(std::string s)
 * --                       s   - string that will be printed to the screen
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function takes a user string and prints it to the console window of the application.
 * ----------------------------------------------------------------------------------------------------------------------*/
void GlobalUIManager::printToTerminal(std::string s)
{
    QString qString;
    qString = QString::fromStdString(s).append("\n");
    ui->consoleWindow->moveCursor(QTextCursor::End);
    ui->consoleWindow->insertPlainText(qString);
}
