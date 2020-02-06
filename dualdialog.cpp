/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: dualdialog.cpp - Provides the user with a graphical interface for resolving Service names and
 * -- port numbers. It requires the user to provide the protocol used by the service to properly resolve the desired
 * -- network lookup. It processes user input by interfacing with the Windows Socket API, made available through
 * -- the WindowsSocketManager class.
 * --
 * -- PROGRAM: resolutionAssignment1
 * --
 * -- FUNCTIONS:
 * -- void configureServiceToPort()
 * -- void configurePortToService()
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
 * -- This class handles all functionality related to dualdialog.ui, created using the Qt framework. Invalid
 * -- inputs will be inherently handled by the Windows Socket API error handling and displayed.
 * ----------------------------------------------------------------------------------------------------------------------*/
#include "dualdialog.h"
#include "ui_dualdialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QTextEdit>


dualdialog::dualdialog(QWidget *parent)
   : QDialog(parent)
   , ui(new Ui::dualdialog)
{
   ui->setupUi(this);
   dialogManager = new DialogManager();
}

dualdialog::~dualdialog()
{
   delete ui;
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: configureServiceToPort()
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void configureServiceToPort()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- Call this function to configure the UI of the dialog box to request the user to enter a service name and
 * -- protocol for network resolution. The ID of the dialog box is also set to 2 to differentiate the lookup to be performed.
 * ----------------------------------------------------------------------------------------------------------------------*/
void dualdialog::configureServiceToPort()
{
   ui->id->setText("1");
   ui->label->setText("Enter Service Name to Resolve to Port Number:");
   ui->label2->setText("Protocol:");
   ui->outputLabel->setText("Port Number:");
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: configurePortToService()
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void configurePortToService()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- Call this function to configure the UI of the dialog box to request the user to enter a valid port number and
 * -- protocol for network resolution. The ID of the dialog box is also set to 2 to differentiate the lookup to be performed.
 * ----------------------------------------------------------------------------------------------------------------------*/
void dualdialog::configurePortToService()
{
   ui->id->setText("2");
   ui->label->setText("Enter Port Number to Resolve to Service Name:");
   ui->label2->setText("Protocol:");
   ui->outputLabel->setText("Service Name:");
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: void on_pushButton_clicked()
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void on_pushButton_clicked()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- This function represents the event handler when the resolution button is clicked within the Qt
 * -- UI. If the ID of the dialog box is 1, then handler will attempt to resolve the service name and the
 * -- protocol to the port number of the service. If the ID of the dialog box is 2, the handler will
 * -- attempt to resolve the Port Number and protocol to the service name.
 * ----------------------------------------------------------------------------------------------------------------------*/
void dualdialog::on_pushButton_clicked()
{
   std::string id  = ui->id->text().toStdString();
   std::string str = ui->input->toPlainText().toStdString();
   const char  *s  = str.c_str();

   std::string str2        = ui->input2->toPlainText().toStdString();
   const char  *protocol_c = str2.c_str();
   QString     output      = id == "1" ?
                             dialogManager->getWinsockManager()->resolveServiceNameToPort(s, protocol_c) :
                             dialogManager->getWinsockManager()->resolvePortToServiceName(s, protocol_c);

   ui->output->setText(output);
}


void dualdialog::on_buttonBox_2_clicked(QAbstractButton *button)
{
   connect(button, SIGNAL(clicked()), this, SLOT(close()));
}
