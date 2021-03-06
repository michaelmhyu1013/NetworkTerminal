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
    Ui::MainWindow *getUI() { return(this->ui); }

    ~MainWindow();

private slots:
    void configureConnection();
    void createConnection(ConnectionConfigurations *connConfig);

    void on_actionResolve_host_Name_To_IP_triggered();
    void on_actionResolve_IP_To_Host_Name_triggered();
    void on_actionResolve_Service_Name_To_Port_triggered();
    void on_actionResolve_Port_To_Service_Name_triggered();
    void on_actionConnect_as_Client_triggered();
    void on_actionConnect_as_Server_triggered();
    void on_actionUpload_File_triggered();
    void on_actionClose_connection_triggered();
};
