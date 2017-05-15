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

#include <vector>
#include <QObject>
#include <QHash>
#include <QMutexLocker>
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

private:
  QString trainName;
  static int totalNbrOfTrains;
  int nbrOfSeats;
  int nbrOfPassengers;
  int currentTrack;//-1 indicates that train is not on a track
  int currentStation;//-1 indicates that train is not on a station
  int positionOnTrack;//[m]
  int currentSpeed;//[m/s]
  int desiredSpeed;//[m/s]
  int maxSpeed;
  int trainID;
  int nextTrack;
  vector<int> travelPlanByStationID;
  int nextIndexTravelPlanByStationID;
  //int lastIndexTravelPlanByStationID;
  QList<Track*> *thisTrackList;
  QList<Train*> *thisTrainList;
  QList<Station*> *thisStationList;
  QMutex mutex;
  QHash<QString, int> stateTable;
  int state;
  int doorOpenProgress;//Doors needs 5 seconds to open and close. 0=closed, 100=open
  bool passing;

public:
  Train(QString name,
        QList<Track*>& trackList,
        QList<Train*>& trainList,
        QList<Station*>& stationList);
  ~Train();
  int addStationToTravelPlan(int stationID);//To be removed and replaced by addStationToTrainRoute
  int getCurrentSpeed();
  int getCurrentStation();
  int getCurrentTrack();
  int getDesiredSpeed();
  int getID();
  QString getName();
  int getTrackPosition();
  int getIndexTravelPlanByStationID();
  vector<int> getTravelPlan();
  //vector<int> getRouteVectorTracks();//To delete
  void load(int n);
  int move(int n);//if train shall wait for other elements, return int>0.

  int waitingState(int n);
  int readyState(int n);
  int runningState(int n);
  int openingState(int n);
  int loadingState(int n);
  int closingState(int n);
  int readyToRunningState(int n);
  int runningToOpeningState(int n);
  /*int runningToReady(int n);
  int waitingToOpening(int n);
  int runningToPassing(int n);
  int passingToEmergency(int n);*/

  void sendDataChangedSignal(int trainID);
  void setCurrentStation(int StationID);
  void setCurrentTrack(int trackID);
  void setName(QString const& tn);
  void setModel(QString const& modelID);
  void setDesiredSpeed(int n);
  void setTrackPosition(int n);
  void showInfo();
};

#endif
