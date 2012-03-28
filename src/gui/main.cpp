#include <QtGui/QApplication>
#include "mainwindow.h"
#include "gcodeviewapplication.h"

int main(int argc, char *argv[])
{
    GCodeViewApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
