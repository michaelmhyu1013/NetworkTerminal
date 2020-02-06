#pragma once

#include "configurationwindow.h"
#include "ConnectionConfigurations.h"
#include "ConnectionManager.h"
#include "WindowsSocketManager.h"
#include <QMainWindow>
QT_BEGIN_NAMESPACE

namespace Ui
{

class MainWindow;

}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow           *ui;
    WindowsSocketManager     *winsockManager;
    ConfigurationWindow      *confWindow;
    ConnectionConfigurations *connConfig;
    ConnectionManager        *connManager;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void configureConnection();
    void on_actionResolve_host_Name_To_IP_triggered();
    void on_actionResolve_IP_To_Host_Name_triggered();
    void on_actionResolve_Service_Name_To_Port_triggered();
    void on_actionResolve_Port_To_Service_Name_triggered();
    void on_actionConnect_as_Client_triggered();
    void on_actionConnect_as_Server_triggered();
};
