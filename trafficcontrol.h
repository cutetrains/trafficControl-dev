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
#include <iostream>//Port to QDebug
#include <sstream>//Port to QDebug
#include "tctrain.h"
#include "tctrack.h"
#include "tcstation.h"
#include "trafficdatamodel.h"
#include "trafficclock.h"


namespace Ui {
    class TrafficControl;
}

class TrafficControl : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit TrafficControl(QWidget *parent = 0);
    ~TrafficControl();
    QList<Train*> trainList;
    QList<Track*> trackList;
    QList<Station*> stationList;
    QThread clockThread;
    TrafficClock trafficClock;

public slots:
    void addTrainToNetworkUI();
    void addTrackToNetwork(QString trackName, int trackLength);
    void addTrainToNetwork(QString trainName);
    void addStationToNetwork(QString stationName);
    void listTracksInNetwork();
    void listTrainsInNetwork();
    void listStationsInNetwork();
    void createPredefinedNetworkUI();
    void importPredefinedNetwork();

    void onRunThreadCheckBoxChanged(int newState);
    void onTickIntervalChanged(int newInterval);

    void stepTimeForNetwork();
    int connectTrackToStations(QString trackName, QString startStationName, QString endStationName);
    int connectTrackToStations(int trackID, int startStationID, int endStationID);
    
private:
    Ui::TrafficControl *ui;
    QStringList list;
    Track *track1;
    Train *train1;
    Station *station1;
    TrafficDataModel *trackListModel;
    TrafficDataModel *trainListModel;
    TrafficDataModel *stationListModel;
    QMutex mutex;

};

#endif // TRAFFICCONTROL_H
