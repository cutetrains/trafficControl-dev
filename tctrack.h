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
#include <QQueue>
#include <QVariant>

#ifndef _TCTRACK_H_
#define _TCTRACK_H_
using namespace std;
class Train;
class Station;
class Track: public QObject// to do later: Add vector with indices to the trains on each track.
{
  Q_OBJECT

signals:
  void dataChangedSignal(int , const QVariant & );
  void qmlTrackStatusSignal(QVariant, QVariant, QVariant);

private:
  static int totalNbrOfTracks;
  int length;
  //int trainsOnTrack;
  QString name;
  int startStation;
  int endStation;
  int maxAllowedSpeed;
  int trackID;
  QQueue<int> trainsOnTrackQueue;

public:
  //TODO: Rename parameters to be more intuitive
  Track(QString cn,
        int nn,
        QList<Track*>& trackList,
        QList<Train*>& trainList,
        QList<Station*>& stationList);//ID and length
  Track(const Track& sTrack);
  Track& operator=(const Track& sTrack);
  ~Track();
  bool addTrain(int trainID);
  bool deleteTrain(int trainID);
  int getID();
  int getLength();
  int getMaxAllowedSpeed();
  int getEndStation();
  QString getName();
  int getStartStation();
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
