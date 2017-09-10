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

#ifndef TRAFFICCONTROL_H
#define TRAFFICCONTROL_H

#include <QMainWindow>
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMutexLocker>
#include <QUrl>
//#include <QQuickView>
#include <QGeoCoordinate>
#include <QtQml>
#include "tctrain.h"
#include "tctrack.h"
#include "tcstation.h"
#include "trafficdatamodel.h"
#include "trafficclock.h"
#include "tcnetworkcontrol.h"

namespace Ui {
  class TrafficControl;
}

class TrafficControl : public QMainWindow
{
  Q_OBJECT
    
public:
  explicit TrafficControl(QWidget *parent = 0);
  ~TrafficControl();
  QThread clockThread;//TCNETWORKCONTROL
  QWidget *mapContainer;
  QList<Station*> stationList;//TCNETWORKCONTROL
  QList<Track*> trackList;//TCNETWORKCONTROL
  QList<Train*> trainList;//TCNETWORKCONTROL
  TrafficClock trafficClock; //TCNETWORKCONTROL

public slots:
  void addTrackToNetwork(QString trackName,
                         int trackLength,
                         QStringList coordinates);
  void addTrainToNetwork(QString trainName);
  void addStationToNetwork(QString stationName,
                           bool isJunction,
                           QString lat,
                           QString lon);
  int connectTrackToStations(QString trackName,
                             QString startStationName,
                             QString endStationName);
  int connectTrackToStations(int trackID,
                             int startStationID,
                             int endStationID);
  int getNumberOfTrains();
  void importPredefinedNetwork();
  void onRunThreadCheckBoxChanged(int newState);
  void onTickIntervalChanged(int newInterval);
  void stepTimeForNetwork();
    
private:
  NetworkControl *networkControl(int i);
  Ui::TrafficControl *ui;
  //QStringList list;
  QObject* handleQMLObject;//TCNETWORKCONTROL
  TrafficDataModel *trackListModel;
  TrafficDataModel *trainListModel;
  TrafficDataModel *stationListModel;
  QMutex mutex;//TCNETWORKCONTROL
};

#endif // TRAFFICCONTROL_H