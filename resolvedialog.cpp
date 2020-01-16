#include "resolveDialog.h"
#include "resolvedialog.h"

ResolveDialog::ResolveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResolveDialog)
{
    ui->setupUi(this);
    dialogManager = new DialogManager();
}

ResolveDialog::~ResolveDialog()
{
    delete ui;
}

void ResolveDialog::configureHostToIP()
{
    ui->label->setText("Enter Host Name to Resolve to IP Address:");
}

void ResolveDialog::configureIPToHost()
{
    ui->label->setText("Enter Address to Resolve to Host Name:");
}

void ResolveDialog::configureServiceToPort()
{
    ui->label->setText("Enter Service Name to Resolve to Port Number:");
}

void ResolveDialog::ConfigurePortToService()
{
    ui->label->setText("Enter Port Number to Resolve to Service Name:");
}
