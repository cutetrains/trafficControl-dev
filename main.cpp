#include "trafficcontrol.h"
#include "trafficclock.h"
#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TrafficControl w;

    w.show();

    return a.exec();
}
