#ifndef CONFIGURATIONWINDOW_H
#define CONFIGURATIONWINDOW_H

#include <QDialog>

namespace Ui
{

class ConfigurationWindow;

}

class ConfigurationWindow : public QDialog
{
   Q_OBJECT

private:
   Ui::ConfigurationWindow *ui;

public:
   explicit ConfigurationWindow(QWidget *parent = nullptr);
   ~ConfigurationWindow();


   Ui::ConfigurationWindow *getConfigurationWindowUI()
   {
      return(ui);
   }

signals:
   void configurationWindowNotify();

private slots:
   void on_okButton_clicked();
};

#endif // CONFIGURATIONWINDOW_H
