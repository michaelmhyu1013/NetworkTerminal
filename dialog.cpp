#include "dialog.h"
#include "ui_dialog.h"

dialog::dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog)
{
    ui->setupUi(this);
    dialogManager = new DialogManager();
}

dialog::~dialog()
{
    delete ui;
}

void dialog::configureHostToIP()
{
    ui->label->setText("Enter Host Name to Resolve to IP Address:");
    ui->outputLabel->setText("IP Address:");
}

void dialog::configureIPToHost()
{
    ui->label->setText("Enter Address to Resolve to Host Name:");
    ui->outputLabel->setText("Host Name:");
}

void dialog::configureServiceToPort()
{
    ui->label->setText("Enter Service Name to Resolve to Port Number:");
    ui->outputLabel->setText("Port Number:");
}

void dialog::ConfigurePortToService()
{
    ui->label->setText("Enter Port Number to Resolve to Service Name:");
    ui->outputLabel->setText("Service Name:");
}


void dialog::on_pushButton_clicked()
{

}
