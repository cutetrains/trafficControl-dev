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
#include <QQueue>
#include <QVariant>
#include <QList>

#ifndef _TCTRACK_H_
#define _TCTRACK_H_
using namespace std;
class Train;
class Station;
class Track: public QObject
{
  Q_OBJECT

signals:
  void dataChangedSignal(int , const QVariant & );
  void qmlTrackStatusSignal(QVariant, QVariant, QVariant);

private:
  QList<float> coordinateList;
  QList<int> coordinateCumulatedDistanceList;
  int endStation;
  int length;
  int maxAllowedSpeed;
  QString name;
  int startStation;
  static int totalNbrOfTracks;
  int trackID;
  QQueue<int> trainsOnTrackQueue;

public:
  Track(QString trackName,
        int trackLength,
        QStringList coordinates,
        QList<Track*>& trackList,
        QList<Train*>& trainList,
        QList<Station*>& stationList);//ID and length
  Track(const Track& sTrack);
  Track& operator=(const Track& sTrack);
  ~Track();
  bool addTrain(int trainID);
  bool deleteTrain(int trainID);
  void destructorResetTotalNumberOfTracks();
  QList<float> getCoordinatesFromPosition(int position);
  int getEndStation();
  int getID();
  int getLength();
  int getMaxAllowedSpeed();
  QString getName();
  int getStartStation();
  int getTotalNbrOfTracks();
  void sendDataChangedSignal(int trackID);
  void setEndStation( int stationID);//To be handled in trafficcontrol
  void setMaxAllowedSpeed(int n);//to implement
  void setStartStation( int stationID);//To be handled in trafficcontrol
  void showInfo();//modify
  QList<Track*> *thisTrackList;//inserted, REMOVE?
  QList<Train*> *thisTrainList;//inserted
  QList<Station*> *thisStationList;//inserted
};
#endif
