/*************************************************************************
 * This file is part of TrafficControl.
 *
 * TrafficControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TrafficControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TrafficControl.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

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
