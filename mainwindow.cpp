/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: mainwindow.cpp -
 * --
 * -- PROGRAM: resolutionAssignment1
 * --
 * -- FUNCTIONS:
 * --
 * --  void on_actionResolve_host_Name_To_IP_triggered
 * --  void on_actionResolve_IP_To_Host_Name_triggered
 * --  void on_actionResolve_Service_Name_To_Port_triggered
 * --  void on_actionResolve_Port_To_Service_Name_triggered
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
 * -- This class handles all user interaction with the main windows of the UI application. Menu item functionalities are
 * -- handled within this class and displays the corresponding dialog box that the user has selected.
 * ----------------------------------------------------------------------------------------------------------------------*/

#include "dialog.h"
#include "dualdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QTextEdit>
#include <QTextStream>
#include <ui_configurationwindow.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    winsockManager = new WindowsSocketManager();
    confWindow     = new ConfigurationWindow(this);
    connManager    = new ConnectionManager();
    connConfig     = new ConnectionConfigurations();
    connect(confWindow, &ConfigurationWindow::configurationWindowNotify, this, &MainWindow::configureConnection);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::configureConnection()
{
    std::string ip             = confWindow->getConfigurationWindowUI()->hostInput->toPlainText().toStdString();
    int         port           = confWindow->getConfigurationWindowUI()->portInput->toPlainText().toInt();
    int         packetSize     = confWindow->getConfigurationWindowUI()->packetValue->currentText().toInt();
    int         transmissions  = confWindow->getConfigurationWindowUI()->tranmissionValue->currentText().toInt();
    int         connectionType = confWindow->getConfigurationWindowUI()->connectionUDP->isChecked() ? 0 : 1;

//    try
//    {
//        const char  *ipBuf      = ip.c_str();
//        std::string convertedIp = this->winsockManager->resolveHostNameToIP(ipBuf)[0];
//        ip = convertedIp;
//    }
//    catch (exception e)
//    {
//    }

    this->connConfig->ip             = &ip;
    this->connConfig->port           = port ? port : 7777;
    this->connConfig->packetSize     = packetSize;
    this->connConfig->transmissions  = transmissions;
    this->connConfig->connectionType = connectionType;
    connManager->createConnection(connConfig);
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: on_actionResolve_host_Name_To_IP_triggered()
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void on_actionResolve_host_Name_To_IP_triggered()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function handles the user event for when the user selects the option to resolve a host name to an IP address.
 * -- It displays the dialog box that awaits user-input for the host name.
 * ----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::on_actionResolve_host_Name_To_IP_triggered()
{
    dialog rDialog;

    rDialog.configureHostToIP();
    rDialog.setModal(true);
    rDialog.exec();
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: on_actionResolve_IP_To_Host_Name_triggered()
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void on_actionResolve_IP_To_Host_Name_triggered()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function handles the user event for when the user selects the option to resolve an IP address to a host name.
 * -- It displays the dialog box that awaits user-input for the IP address.
 * ----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::on_actionResolve_IP_To_Host_Name_triggered()
{
    dialog rDialog;

    rDialog.configureIPToHost();
    rDialog.setModal(true);
    rDialog.exec();
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: on_actionResolve_Service_Name_To_Port_triggered()
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void on_actionResolve_Service_Name_To_Port_triggered()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function handles the user event for when the user selects the option to resolve a service name and protocol
 * -- to a port number. It displays the dialog box that awaits user-input for the two inputs.
 * ----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::on_actionResolve_Service_Name_To_Port_triggered()
{
    dualdialog rDialog;

    rDialog.configureServiceToPort();
    rDialog.setModal(true);
    rDialog.exec();
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: on_actionResolve_Port_To_Service_Name_triggered()
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void on_actionResolve_Port_To_Service_Name_triggered()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function handles the user event for when the user selects the option to resolve a port number and protocol
 * -- to a service name. It displays the dialog box that awaits user-input for the two inputs.
 * ----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::on_actionResolve_Port_To_Service_Name_triggered()
{
    dualdialog rDialog;

    rDialog.configurePortToService();
    rDialog.setModal(true);
    rDialog.exec();
}


void MainWindow::on_actionConnect_as_Client_triggered()
{
    confWindow->show();
    connConfig->connectionMode = ConnectionConfigurations::CONNECT_MODE::CLIENT;
}


void MainWindow::on_actionConnect_as_Server_triggered()
{
    confWindow->show();
    connConfig->connectionMode = ConnectionConfigurations::CONNECT_MODE::SERVER;
}
