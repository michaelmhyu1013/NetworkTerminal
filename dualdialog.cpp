#include "dualdialog.h"
#include "ui_dualdialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QTextEdit>


dualdialog::dualdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dualdialog)
{
    ui->setupUi(this);
    dialogManager = new DialogManager();
}

dualdialog::~dualdialog()
{
    delete ui;
}
void dualdialog::configureServiceToPort()
{
    ui->id->setText("1");
    ui->label->setText("Enter Service Name to Resolve to Port Number:");
    ui->label2->setText("Protocol:");
    ui->outputLabel->setText("Port Number:");
}

void dualdialog::configurePortToService()
{
    ui->id->setText("2");
    ui->label->setText("Enter Port Number to Resolve to Service Name:");
    ui->label2->setText("Protocol:");
    ui->outputLabel->setText("Service Name:");
}

void dualdialog::on_pushButton_clicked()
{
    std::string id = ui->id->text().toStdString();
    std::string str = ui->input->toPlainText().toStdString();
    const char *s = str.c_str();

    std::string str2 = ui->input2->toPlainText().toStdString();
    const char *protocol_c = str2.c_str();
    QString output = id == "1" ?
                dialogManager->getWinsockManager()->resolveServiceNameToPort(s, protocol_c):
                dialogManager->getWinsockManager()->resolvePortToServiceName(s, protocol_c);
    ui->output->setText(output);

}
