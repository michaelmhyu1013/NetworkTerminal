#pragma once

#include <QMainWindow>
#include "windowssocketmanager.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    WindowsSocketManager *winsockManager;

public:

    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void on_actionResolve_host_Name_To_IP_triggered();

    void on_actionResolve_IP_To_Host_Name_triggered();

    void on_actionResolve_Service_Name_To_Port_triggered();

    void on_actionResolve_Port_To_Service_Name_triggered();

};
