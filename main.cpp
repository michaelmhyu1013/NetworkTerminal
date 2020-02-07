/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: main.cpp - An application that provides the user with a menu-driven application that demonstrates
 * --                          the basic usages of the Windows Socket API. It allows the user to resolve the desired
 * --                          network lookup values, using a combination of the following parameters:
 * --                          Host Name, IP Address, Service Name, and Protocol
 * --
 * --
 * -- PROGRAM: resolutionAssignment1
 * --
 * -- FUNCTIONS:
 *
 * -- int main(int argc, char *argv[])
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
 * -- This class is the entry point for the Qt application. It generates a menu-driven interface for the user
 * -- to perform network resolution actions.
 * ----------------------------------------------------------------------------------------------------------------------*/

#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow   w;

    w.show();
    return(a.exec());
}
