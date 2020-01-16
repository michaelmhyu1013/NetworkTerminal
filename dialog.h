#pragma once

#include "dialogmanager.h"

#include <QDialog>

namespace Ui {
class dialog;
}

class dialog : public QDialog
{
    Q_OBJECT

private:
    Ui::dialog *ui;
    DialogManager* dialogManager;

public:
    explicit dialog(QWidget *parent = nullptr);

    ~dialog();

    void configureHostToIP();

    void configureIPToHost();

    void configureServiceToPort();

    void ConfigurePortToService();
private slots:
    void on_pushButton_clicked();

};
