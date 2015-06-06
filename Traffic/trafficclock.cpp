#include "trafficclock.h"
#include <QDebug>


TrafficClock::TrafficClock(QObject *parent) :
    QObject(parent)
{
    continueTick=false;//hardcoded, let the trafficControl emit a signal that updates this parameter later
    isAlive=true;
    systemTime=QTime::currentTime();//may be deleted
    timer.start();
    tInterval=500;
}

void TrafficClock::setTickInterval(int n)
{
    tInterval=n;
}

void TrafficClock::disconnectThread()
{
    isAlive=false;
}

/*void TrafficClock::releaseMutex()
{
   //threadMutexLocked=false;
}*/

/*!
 * 2014-06-22 Gustaf
* The is the is called by the thread. This is called everythime the counter is activated by the user.
* How to pause the thread?
*/
void TrafficClock::threadTick()
{
    targetTime=QTime::currentTime();
    while(isAlive)
    {
        targetTime=targetTime.addMSecs(tInterval);
        tWait=std::max(QTime::currentTime().msecsTo(targetTime),tInterval/2);
        if(tWait<0){qDebug()<<"***ERROR*** Target time missed!";}
        if(continueTick==true)
        {
            emit stepTimeSignal();
        }

        QThread::msleep(std::max(tWait, tInterval/2));
    }
}


void TrafficClock::pauseThread()
{
    continueTick=false;
}

void TrafficClock::resumeThread()
{
    continueTick=true;
}

void TrafficClock::threadSetup(QThread &clockThread)
{
    connect(&clockThread, SIGNAL(started()), this, SLOT(threadTick()) );
}

TrafficClock::~TrafficClock()
{
}
