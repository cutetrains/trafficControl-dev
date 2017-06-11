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
#ifndef _TCSTATION_H_
#define _TCSTATION_H_
using namespace std;
class Track;
class Train;
class Station: public QObject
{
	Q_OBJECT

signals:
  void dataChangedSignal(int, const QVariant & );
  //void stationChangedSignal(int , const QVariant &, int);//2017-06 MODIFY STATION
  void stationChangedSignal();//2017-06 MODIFY STATION
private:
  static int totalNbrOfStations;
  int waitingPassengers;
  QString name;
  int stationID;
  vector<int> leavingTrackVector;
  QList<Track*> *thisTrackList;//inserted, REMOVE?
  QList<Train*> *thisTrainList;//inserted
  QList<Station*> *thisStationList;//inserted

public:// to do later: Add vector with pointers to the trains on each station.
  /*! The constructor for a Station object.*/
  Station(QString cn,
          bool isJunction,
          QList<Track*>& trackList,
          QList<Train*>& trainList,
          QList<Station*>& stationList);
  ~Station();

  bool checkIfTrackLeavesStation(int trackID);
  int findLeavingTrackIndexToStation(int targetStationID);
  void addTrack(int trackID); //To be handled in trafficcontrol
  void trainArrival(int trainID);
  void changeNbrOfPassengers(int n);
  int getID();
  QString getName();
  int getNbrOfWaitingPassengers();
  void sendDataChangedSignal(int stationID);
  void showInfo();
  bool isJunction;
};

#endif
