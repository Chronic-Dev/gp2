#include <QtGui/QApplication>
#include "gpmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GPMainWindow w;
    w.show();

    return a.exec();
}
