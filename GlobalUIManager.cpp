#include <GlobalUIManager.h>

void GlobalUIManager::printToTerminal(std::string s)
{
    QString qString;
    qString = QString::fromStdString(s).append("\n");
    ui->consoleWindow->moveCursor(QTextCursor::End);
    ui->consoleWindow->insertPlainText(qString);
}
