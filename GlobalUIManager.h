#pragma once
#include <ui_mainwindow.h>

class GlobalUIManager
{
private:
    GlobalUIManager() {}
    Ui::MainWindow *ui;

public:
    static GlobalUIManager &getInstance()
    {
        static GlobalUIManager theInstance;

        return(theInstance);
    }

    inline void setUIObject(Ui::MainWindow *ui) { this->ui = ui; }

    void printToTerminal(std::string s);
};
