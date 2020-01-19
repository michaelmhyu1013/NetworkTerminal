#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "dualdialog.h"
#include <QDialog>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->winsockManager = new WindowsSocketManager();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionResolve_host_Name_To_IP_triggered()
{
    dialog rDialog;
    rDialog.configureHostToIP();
    rDialog.setModal(true);
    rDialog.exec();
}

void MainWindow::on_actionResolve_IP_To_Host_Name_triggered()
{
    dialog rDialog;
    rDialog.configureIPToHost();
    rDialog.setModal(true);
    rDialog.exec();
}

void MainWindow::on_actionResolve_Service_Name_To_Port_triggered()
{
    dualdialog rDialog;
    rDialog.configureServiceToPort();
    rDialog.setModal(true);
    rDialog.exec();
}

void MainWindow::on_actionResolve_Port_To_Service_Name_triggered()
{
    dualdialog   rDialog;
    rDialog.configurePortToService();
    rDialog.setModal(true);
    rDialog.exec();
}
