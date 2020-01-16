#pragma once

#include "mainwindow.h"
#include "dialogmanager.h"
#include "windowssocketmanager.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ResolveDialog; }
QT_END_NAMESPACE

class ResolveDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::ResolveDialog *ui;
    DialogManager* dialogManager;

public:
    explicit ResolveDialog(QWidget *parent = nullptr);

    ~ResolveDialog();

    void configureHostToIP();

    void configureIPToHost();

    void configureServiceToPort();

    void ConfigurePortToService();
private slots:

};
