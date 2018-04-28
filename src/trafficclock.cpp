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
  simulatedTime = QTime::currentTime();
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
 * Increments system time
 *
 * @param iSeconds time to add
 */
void TrafficClock::incrementSimulatedTime(int iSeconds)
{
  simulatedTime = simulatedTime.addSecs(iSeconds);
  emit updateSimulatedTimeSignal(simulatedTime.toString());
}

/*!
* The is the is called by the thread. This is called everythime the counter is activated by the user.
*
*/
void TrafficClock::threadTick()
{
  targetTime = QTime::currentTime();
  int nrOfAdditionsThisTick;
  while(isAlive)
  {
    //qDebug()<<QTime::currentTime()<<"Thread resumed"; //@HIDE IF RELEASE

    if (true == simulationIsRunning)
    {
      emit stepTimeSignal();
    }
    /*First, find a target time in the future*/
    nrOfAdditionsThisTick = 0;
    do{
      nrOfAdditionsThisTick++;
      targetTime = targetTime.addMSecs(tInterval);
      //qDebug()<<"Added "<<tInterval<<" to target. New Target: "<<targetTime;
    } while (targetTime < QTime::currentTime());

    if(nrOfAdditionsThisTick > 1){qDebug()<<"Target time missed ";}
    //qDebug()<<QTime::currentTime()<<"Thread sleep";  //@HIDE IF RELEASE

    QThread::msleep(QTime::currentTime().msecsTo(targetTime));
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
