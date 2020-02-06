#include "configurationwindow.h"
#include "ui_configurationwindow.h"

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
