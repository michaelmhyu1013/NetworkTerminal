#include "configurationwindow.h"
#include "ui_configurationwindow.h"

/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE:		ConfigurationWindow.cpp -	QT class responsible for handling user interaction with the
 * --                                               ConfigurationWindow form to emit signals.
 * --
 * -- PROGRAM:			NetworkTerminal
 * --
 * -- DATE:             Feb 5, 2020
 * --
 * -- FUNCTIONS:
 * --                   int closeSockets()
 * --                   int readFileToOutputBuffer(FileUploadStruct *fs)
 * -- REVISIONS:
 * --		NA
 * --
 * -- DESIGNER:         Michael Yu
 * --
 * -- PROGRAMMER:		Michael Yu
 * --
 * -- NOTES:
 * -- QT class associated with a UI form that captures user interaction with the application. This form captures the user
 * -- selected input for the Client and Server connection such as IP, Packet Size, Transmissions, and Port.
 * ----------------------------------------------------------------------------------------------------------------------*/
ConfigurationWindow::ConfigurationWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConfigurationWindow)
{
    ui->setupUi(this);
}

ConfigurationWindow::~ConfigurationWindow()
{
    delete ui;
}


void ConfigurationWindow::on_okButton_clicked()
{
    configurationWindowNotify();
    this->close();
}
