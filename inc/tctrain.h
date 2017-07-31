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

#include <QObject>
#include <QHash>
#include <QMutexLocker>
#include <QVariant>
#include <QList>
#ifndef _TCTRAIN_H_
#define _TCTRAIN_H_
using namespace std;
class Track;
class Station;

class Train: public QObject
{
  Q_OBJECT

signals:
  void dataChangedSignal(int , const QVariant & );
  void qmlTrainPositionSignal(QVariant, QVariant, QVariant);

private:
  int currentStation;//-1 indicates that train is not on a station
  int currentSpeed;//[m/s]
  int currentTrack;//-1 indicates that train is not on a track
  int desiredSpeed;//[m/s]
  int doorOpenProgress;//Doors needs 5 seconds to open and close. 0=closed, 100=open
  int positionOnTrack;//[m]
  int maxSpeed;
  QMutex mutex;
  int nextIndexTravelPlanByStationID;
  int nextTrack;
  int nbrOfPassengers;
  int nbrOfSeats;
  bool passing;
  int state;
  QHash<QString, int> stateTable;
  static int totalNbrOfTrains;
  QList<float> trainCoordinates;
  int trainID;
  QString name;
  QList<int> travelPlanByStationID;
  QList<Track*> *thisTrackList;
  QList<Train*> *thisTrainList;
  QList<Station*> *thisStationList;

public:
  Train(QString trainName,
        QList<Track*>& trackList,
        QList<Train*>& trainList,
        QList<Station*>& stationList);
  ~Train();
  int addStationToTravelPlan(int stationID);//To be removed and replaced by addStationToTrainRoute
  int closingState(int n);
  int getCurrentSpeed();
  int getCurrentStation();
  int getCurrentTrack();
  int getDesiredSpeed();
  int getID();
  int getIndexTravelPlanByStationID();
  QString getName();
  int getTrackPosition();
  QList<int> getTravelPlan();
  void load(int n);
  int loadingState(int n);
  int move(int n);//if train shall wait for other elements, return int>0.
  int openingState(int n);
  int readyState(int n);
  int readyToRunningState(int n);
  int runningState(int n);
  int runningToOpeningState(int n);
  int waitingState(int n);
  void sendDataChangedSignal(int trainID);
  void setCurrentStation(int StationID);
  void setCurrentTrack(int trackID);
  void setName(QString trainName);
  void setModel(QString const& modelID);
  void setDesiredSpeed(int n);
  void setTrackPosition(int n);
  void showInfo();
};

#endif
