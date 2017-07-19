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
  continueTick = false;//hardcoded, let the trafficControl emit a signal that updates this parameter later
  isAlive = true;
  systemTime = QTime::currentTime();//may be deleted
  timer.start();
  tInterval = 500; //No magic numbers
}

void TrafficClock::setTickInterval(int n)
{
  tInterval = n;
}

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
    tWait = max(QTime::currentTime().msecsTo(targetTime) , tInterval/2);
    if (tWait < 0)
    {
      qDebug()<<"***ERROR*** Target time missed!";
    }
    if (true == continueTick)
    {
      emit stepTimeSignal();
    }
    QThread::msleep(max(tWait, tInterval/2));
  }
}

void TrafficClock::pauseThread()
{
  continueTick = false;
}

void TrafficClock::resumeThread()
{
  continueTick = true;
}

void TrafficClock::threadSetup(QThread &clockThread)
{
  connect(&clockThread, SIGNAL(started()), this, SLOT(threadTick()) );
}

TrafficClock::~TrafficClock()
{
}
