#pragma once
#include "dialogmanager.h"

#include <QAbstractButton>
#include <QDialog>

namespace Ui
{

class dualdialog;

}

class dualdialog : public QDialog
{
   Q_OBJECT

public:
   explicit dualdialog(QWidget *parent = nullptr);

   ~dualdialog();

   void configureServiceToPort();

   void configurePortToService();

private slots:
   void on_pushButton_clicked();

   void on_buttonBox_2_clicked(QAbstractButton *button);

private:
   Ui::dualdialog *ui;
   DialogManager  *dialogManager;
};
