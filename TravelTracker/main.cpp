#include "mainwindow.h"

#include "tests.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //TravelManager();

    Tests::TestSvg1();
    return a.exec();
}
