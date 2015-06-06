#ifndef TRAFFICCLOCK_H
#define TRAFFICCLOCK_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTime>

class TrafficClock : public QObject
{
    Q_OBJECT
public:
    explicit TrafficClock(QObject *parent = 0);
    void threadSetup(QThread &clockThread);
    ~TrafficClock();

signals:
    void stepTimeSignal();

public slots:
    void threadTick();
    void resumeThread();
    void pauseThread();
    void setTickInterval(int);
//    void releaseMutex();
    void disconnectThread();

private:
    bool continueTick; //This shall be toggled to tell whether to continue or not
    bool isAlive; //This shall be enabled until the thread shall be killed.
    bool threadMutexLocked;
    QTime systemTime;
    QTime targetTime;
    QTime simulatedTime; //The simulated time of the train network
    QTime timer;//this one measures time
    int tInterval;
    int tWait;
};

#endif // TRAFFICCLOCK_H
