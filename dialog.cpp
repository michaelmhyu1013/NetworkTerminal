/*------------------------------------------------------------------------------------------------------------------
 * -- SOURCE FILE: dialog.cpp - Provides the user with a graphical interface for resolving IP Address and
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
#include "dialog.h"
#include "ui_dialog.h"


dialog::dialog(QWidget *parent)
   : QDialog(parent)
   , ui(new Ui::dialog)
{
   ui->setupUi(this);
   dialogManager = new DialogManager();
}

dialog::~dialog()
{
   delete ui;
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: configureHostToIP
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void configureHostToIP()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- Call this function to configure the UI of the dialog box to request the user to enter a valid host name
 * -- for network resolution. The ID of the dialog box is also set to 1 to differentiate the lookup to be performed.
 * ----------------------------------------------------------------------------------------------------------------------*/
void dialog::configureHostToIP()
{
   ui->id->setText("1");
   ui->label->setText("Enter Host Name to Resolve to IP Address:");
   ui->outputLabel->setText("IP Address:");
}


/*------------------------------------------------------------------------------------------------------------------
 * -- FUNCTION: configureIPToHost()
 * --
 * -- DATE: Jan 13, 2020
 * --
 * -- REVISIONS: N/A
 * --
 * -- DESIGNER: Michael Yu
 * --
 * -- PROGRAMMER: Michael Yu
 * --
 * -- INTERFACE: void configureIPToHost()
 * --
 * -- RETURNS: void
 * --
 * -- NOTES:
 * -- Call this function to configure the UI of the dialog box to request the user to enter a valid IP address
 * -- for network resolution. The ID of the dialog box is also set to 2 to differentiate the lookup to be performed.
 * ----------------------------------------------------------------------------------------------------------------------*/
void dialog::configureIPToHost()
{
   ui->id->setText("2");
   ui->label->setText("Enter Address to Resolve to Host Name:");
   ui->outputLabel->setText("Host Name:");
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
 * -- UI. If the ID of the dialog box is 1, then handler will attempt to resolve the Host Name to the IP
 * -- address. If the ID of the dialog box is 2, the handler will attempt to resolve the IP Address to the
 * -- host name.
 * ----------------------------------------------------------------------------------------------------------------------*/
void dialog::on_pushButton_clicked()
{
   std::string     id    = ui->id->text().toStdString();
   std::string     input = ui->input->toPlainText().toStdString();
   const char      *s    = input.c_str();
   vector<std::string> values;
   std::string         alias, output;

   if (id == "1")
   {
      values = dialogManager->getWinsockManager()->resolveHostNameToIP(s);
      output = values.at(0);

      if (values.size() > 1)
      {
         alias = values.at(1);
      }
   }
   else
   {
      output = dialogManager->getWinsockManager()->resolveIPToHostName(s);
   }
   ui->output->setText(QString::fromStdString(output));
   ui->alias->setText(QString::fromStdString(alias));
}


void dialog::on_buttonBox_2_clicked(QAbstractButton *button)
{
   connect(button, SIGNAL(clicked()), this, SLOT(close()));
}
