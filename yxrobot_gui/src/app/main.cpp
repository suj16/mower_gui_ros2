#include "mainwindow/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<RobotPose>("RobotPose");
    qRegisterMetaType<OccupancyMap>("OccupancyMap");
    MainWindow w;
    w.show();
    return a.exec();
}
