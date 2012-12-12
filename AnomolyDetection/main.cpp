#include <QtGui/QApplication>
#include "MainWindow.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    try {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
    } catch (...) {
        qDebug() << "Main catch";
        return 0;
    }
    

}
