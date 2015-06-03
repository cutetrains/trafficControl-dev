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
