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
    ui->id->setText("1");
    ui->label->setText("Enter Host Name to Resolve to IP Address:");
    ui->outputLabel->setText("IP Address:");
}

void dialog::configureIPToHost()
{
    ui->id->setText("2");
    ui->label->setText("Enter Address to Resolve to Host Name:");
    ui->outputLabel->setText("Host Name:");
}


void dialog::on_pushButton_clicked()
{
    std::string id = ui->id->text().toStdString();
    std::string input = ui->input->toPlainText().toStdString();
    const char *s = input.c_str();

    QString output = id == "1" ?
                dialogManager->getWinsockManager()->resolveHostNameToIP(s):
                dialogManager->getWinsockManager()->resolveIPToHostName(s);
    ui->output->setText(output);
}
