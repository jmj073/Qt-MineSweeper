#include "mainwindow.h"

#include <QApplication>

// !!write comments(?) - < - zoo stone

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
