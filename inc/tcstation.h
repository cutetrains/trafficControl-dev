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
#include <QVariant>
#include <QtMath>
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
  void qmlStationOccupancySignal(QVariant, QVariant, QVariant);

private:
  bool hasValidCoordinates;
  QString name;
  QList<int> leavingTrackList;
  int numberOfPlatforms;
  int stationID;
  bool stationIsJunction;
  QList<float> thisCoordinate;
  QList<Track*> *thisTrackList;
  QList<Train*> *thisTrainList;
  QList<Station*> *thisStationList;
  static int totalNbrOfStations;
  QList<int> trainsAtStationList;
  int waitingPassengers;

public:
  Station(QString stationName,
          bool isJunction,
          QString latitude,
          QString longitude,
          QList<Track*>& trackList,
          QList<Train*>& trainList,
          QList<Station*>& stationList);
  ~Station();

  void addTrack(int trackID); //To be handled in trafficcontrol
  void changeNbrOfPassengers(int n);
  bool checkIfTrackLeavesStation(int trackID);
  int findLeavingTrackIndexToStation(int targetStationID);
  int getID();
  float getLatitude();
  float getLongitude();
  QString getName();
  QList<int> getLeavingTrackList();
  int getNbrOfPlatforms();
  int getTotalNbrOfStations();
  QList<int> getTrainList();
  int getNbrOfWaitingPassengers();
  bool hasCoordinates();
  bool isJunction();
  void destructorResetTotalNumberOfStations();
  void sendDataChangedSignal(int stationID);
  bool setNbrOfPlatforms(int nbrOfPlatforms);
  void showInfo();
  bool trainArrival(int trainID);
  bool trainDeparture(int trainID);
};

#endif
