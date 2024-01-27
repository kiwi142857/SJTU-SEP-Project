#include <QApplication>
#include <iostream>

#include "map.h"
#include "mymainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyMainWindow my;

    return a.exec();
}
// QTEST_MAIN(LinkTest);
