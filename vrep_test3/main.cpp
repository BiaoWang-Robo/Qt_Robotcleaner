#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    MainWindow m;
    MainWindow n;
    n.show();
    w.show();
    m.show();
    return a.exec();
}

