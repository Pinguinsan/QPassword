#include <memory>
#include <QString>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QPoint>
#include <QMessageBox>

#include "mainwindow.h"
#include <pythoncrypto.h>

int main(int argc, char *argv[])
{
    QApplication qApplication(argc, argv);
    MainWindow mainWindow;
    mainWindow.setWindowIcon(QIcon(MainWindow::MAIN_WINDOW_ICON_PATH));
    mainWindow.show();
    QDesktopWidget qd;
    QRect available = qd.availableGeometry();
    int x = (int)((available.width() / 2) - (mainWindow.width()) / 2);
    int y = (int)((available.height() / 2) + (mainWindow.height()) / 2);
    QPoint topLeft = QPoint(x, y-200);
    x = (int)((available.width() / 2) + (mainWindow.width() / 2));
    y = (int)((available.height() / 2) - (mainWindow.height() / 2));
    QPoint bottomRight = QPoint(x, y-200);
    mainWindow.setGeometry(QRect(topLeft, bottomRight));
    mainWindow.setFixedHeight(mainWindow.height());
    return qApplication.exec();

}
