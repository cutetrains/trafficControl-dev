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

#ifndef TCNETWORKCONTROL_H
#define TCNETWORKCONTROL_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMutexLocker>
#include <QUrl>
#include <QGeoCoordinate>
#include <QtQml>
#include "tctrain.h"
#include "tctrack.h"
#include "tcstation.h"
#include "trafficdatamodel.h"
#include "trafficclock.h"

class NetworkControl : public QObject
{
  Q_OBJECT
    
public:
  NetworkControl(TrafficDataModel &trackListModel,
                 TrafficDataModel &trainListModel,
                 TrafficDataModel &stationListModel,
                 QObject &handleQMLObject);
  ~NetworkControl();

  QThread clockThread;
  QList<Station*> stationList;
  QList<Track*> trackList;
  QList<Train*> trainList;
  TrafficClock trafficClock;

public slots:
  bool addTrackToNetwork(QString trackName,
                         int trackLength,
                         QStringList coordinates);
  void addTrainToNetwork(QString trainName);
  void addStationToNetwork(QString stationName,
                           bool isJunction,
                           QString lat,
                           QString lon);
  bool connectTrackToStationsByName(QString trackName,
                             QString startStationName,
                             QString endStationName);
  /* Two functions that are doing the same thing. Simplify */
  bool connectTrackToStations(int trackID,
                             int startStationID,
                             int endStationID);
  bool parseCmd(QString inputLine);
  bool parseMultipleNetworkCommand(QStringList inputLines);
  void onRunThreadCheckBoxChanged(int newState);
  void onTickIntervalChanged(int newInterval);
  void stepTimeForNetwork();
    
private:
  QObject* handleQMLObject;
  TrafficDataModel *trackListModel;
  TrafficDataModel *trainListModel;
  TrafficDataModel *stationListModel;
  QMutex mutex;

  int cmdParserCurrentTrain;
  int cmdParserCurrentStation;
  int cmdParserCurrentTrack;
};

#endif // TCNETWORKCONTROL_H
