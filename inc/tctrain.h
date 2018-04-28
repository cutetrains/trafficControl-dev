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
  int currentSpeed;
  int currentStation;
  int currentTrack;
  int desiredSpeed;
  int doorOpenProgress;
  bool hasValidCoordinates;
  int maxSpeed;
  int positionOnTrack;
  QMutex mutex;
  QString name;
  int nextIndexTravelPlanByStationID;
  int nextTrack;
  int nbrOfPassengers;
  int passengerCapacity;
  bool passing;
  int state;
  QHash<QString, int> stateTable;
  static int totalNbrOfTrains;
  QList<float> trainCoordinates;
  int trainID;
  QList<int> travelPlanByStationID;
  QList<Track*> *thisTrackList;
  QList<Train*> *thisTrainList;
  QList<Station*> *thisStationList;
  int waitingTimer;

public:
  Train(QString trainName,
        QList<Track*>& trackList,
        QList<Train*>& trainList,
        QList<Station*>& stationList);
  ~Train();
  bool addStationToTravelPlan(int stationID);//To be removed and replaced by addStationToTrainRoute
  int closingState(int n);
  void destructorResetTotalNumberOfTrains();
  int getCurrentSpeed();
  int getCurrentStation();
  int getCurrentTrack();
  int getDesiredSpeed();
  int getID();
  int getIndexTravelPlanByStationID();
  QString getName();
  int getNbrOfPassengers();
  int getPassengerCapacity();
  int getState();
  int getTotalNbrOfTrains();
  int getTrackPosition();
  QList<int> getTravelPlan();
  int load(int nbrOfPassengersToLoad);
  int loadingState(int n);
  int move(int n);
  int openingState(int n);
  int readyState(int n);
  int readyToRunningState(int n);
  int runningState(int n);
  int runningToOpeningState(int n);
  void sendDataChangedSignal(int trainID);
  void setCurrentStation(int StationID);
  void setCurrentTrack(int trackID);
  void setDesiredSpeed(int newDesiredSpeed);
  void setName(QString trainName);
  void setModel(QString const& modelID);
  void setTrackPosition(int n);
  void showInfo();
  int waitingState(int n);
};

#endif
