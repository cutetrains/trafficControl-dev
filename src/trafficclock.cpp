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

#include "../inc/trafficclock.h"
#include <QDebug>
using namespace std;

TrafficClock::TrafficClock(QObject *parent) :
    QObject(parent)
{
  simulationIsRunning = false;//hardcoded, let the trafficControl emit a signal that updates this parameter later
  isAlive = true;
  systemTime = QTime::currentTime();//may be deleted
  timer.start();
  tInterval = 1000; //No magic numbers
}

/*!
* Specifies how often the clock will tick
*
* @param n Interval in seconds
*/
void TrafficClock::setTickInterval(int n)
{
  tInterval = n;
}

/*!
* Stops thread
*/
void TrafficClock::disconnectThread()
{
  isAlive = false;
}


/*!
* The is the is called by the thread. This is called everythime the counter is activated by the user.
*
*/
void TrafficClock::threadTick()
{
  targetTime = QTime::currentTime();
  while(isAlive)
  {
    targetTime = targetTime.addMSecs(tInterval);

    if (true == simulationIsRunning)
    {
      emit stepTimeSignal();
    }
    tWait = QTime::currentTime().msecsTo(targetTime);
    if(tWait<0){
        qDebug()<<"tWait negative. Replacing with tInterval and resetting targetTime";
        tWait = tInterval;
        targetTime = QTime::currentTime().addMSecs(tInterval);
    }
    QThread::msleep(tWait);
  }
}

/*!
* Pauses thread
*/
void TrafficClock::pauseThread()
{
  simulationIsRunning = false;
}

/*!
* Resumes thread
*/
void TrafficClock::resumeThread()
{
  simulationIsRunning = true;
}

/*!
* Connects thread to slot
*
* @param Clock thread to start
*/
void TrafficClock::threadSetup(QThread &clockThread)
{
  connect(&clockThread, SIGNAL(started()), this, SLOT(threadTick()) );
}

TrafficClock::~TrafficClock()
{
}
