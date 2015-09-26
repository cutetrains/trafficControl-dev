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

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include "TrafficControl.h"
#include "ui_TrafficControl.h"
#define TRACK 1
#define TRAIN 2
#define STATION 3
#define TRUE 1
#define FALSE 0
using namespace std;


TrafficControl::TrafficControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TrafficControl)
{
    ui->setupUi(this);

    trafficClock.threadSetup(clockThread);// Use the old implementation of clockThread (see Traffic - R1C012_Independent_Thread)
    trafficClock.moveToThread(&clockThread);
    clockThread.start();

    connect( ui->createTrainButton, SIGNAL( clicked() ), this, SLOT(addTrainToNetworkUI() ) );
    connect( ui->generateNetworkButton, SIGNAL( clicked() ), this, SLOT(createPredefinedNetworkUI() ) );
    connect( ui->importNetworkButton, SIGNAL( clicked() ), this, SLOT(importPredefinedNetwork() ) );
    connect( ui->listTrackButton, SIGNAL( clicked() ), this, SLOT(listTracksInNetwork() ) );
    connect( ui->listTrainButton, SIGNAL( clicked() ), this, SLOT(listTrainsInNetwork() ) );
    connect( ui->listStationButton, SIGNAL( clicked() ), this, SLOT(listStationsInNetwork() ) );

    connect( ui->timeTickButton, SIGNAL( clicked() ), this, SLOT(stepTimeForNetwork() ) );
    connect(&trafficClock, SIGNAL( stepTimeSignal() ), this, SLOT(stepTimeForNetwork() ) );

    connect( ui->runThreadCheckBox, SIGNAL( stateChanged(int) ), this, SLOT(onRunThreadCheckBoxChanged(int)) );
    connect( ui->tickIntervalSpinBox, SIGNAL( valueChanged(int) ), this, SLOT(onTickIntervalChanged(int)) );
    //THE CONNECT SIGNAL/SLOT is not valid


    trackListModel=new TrafficDataModel(TRACK, 0);//QGUSBRA: This instead of 0
    trainListModel=new TrafficDataModel(TRAIN, 0);//QGUSBRA: This instead of 0
    stationListModel=new TrafficDataModel(STATION, 0);//QGUSBRA: This instead of 0
    ui->trackListTableView->setModel( trackListModel);
    ui->trackListTableView->show();
    ui->trainListTableView->setModel( trainListModel );
    ui->trainListTableView->show();
    ui->stationListTableView->setModel( stationListModel );
    ui->stationListTableView->show();
}


/*!
 * The method creates a Track Object, connects it to the onDataChanged slot and appends it to the trackList and the trackListModel.
 *
 * @param trackName Name of the track
 * @param trackLength Length of the track [Unit: m]
 */
void TrafficControl::addTrackToNetwork(QString trackName, int trackLength)
{
    Track* track1 = new Track(trackName, trackLength, trackList, trainList, stationList);
    connect( track1, SIGNAL( dataChangedSignal(int, const QVariant & ) ), trackListModel, SLOT(onDataChanged(int , const QVariant & ) ) );
    trackList.append(track1);
    trackListModel->insertRows(trackList.size(), 1 , QModelIndex());
    track1=NULL;
}

/*!
 * The method addTrackToNetwork creates a Train object with parameters from the User Interface. It connects the train object to the onDataChanged slot and appends it to the trainList and the trainListModel.
 */
void TrafficControl::addTrainToNetworkUI()
{
    qDebug()<<"This function has intentionally been removed";
}

/*!
 * The method addTrainToNetwork creates a Train object, connects it to the onDataChanged slot and appends it to the trainList and the trainListModel.
 *
 * @param nbrOfPassengers Maximum allowed number of passengers. COMMENT: This naming is confusing and should be maxNbrOfPassengers.
 */
void TrafficControl::addTrainToNetwork(QString trainName)// Ambigous variable name. Should be maxNbrOfPassengers.
{
    Train* train1 =new Train(trainName, trackList, trainList, stationList);
    connect( train1, SIGNAL( dataChangedSignal(int, const QVariant & ) ), trainListModel, SLOT(onDataChanged(int , const QVariant & ) ) );
    trainList.append(train1);

    trainListModel->insertRows(trainList.size(), 1 , QModelIndex());
    trainList[trainList.size()-1]->load(2);/*Maybe send datachanged? 2 refers to number
        of passengers and is hardcoded. Change this later*/
    train1 = NULL;
}

/*!
 * The method addStationToNetwork creates a Train object, connects it to the onDataChanged slot and appends it to the trainList and the trainListModel.
 *
 * @param stationName Name of the station
 */
void TrafficControl::addStationToNetwork(QString stationName)
{
    Station* station1 = new Station(stationName, trackList, trainList, stationList);
    connect( station1, SIGNAL( dataChangedSignal(int, const QVariant & ) ), stationListModel, SLOT(onDataChanged(int , const QVariant & ) ) );
    stationList.append(station1);
    stationListModel->insertRows(stationList.size(), 1 , QModelIndex());
    stationList[stationList.size()-1]->changeNbrOfPassengers(0);
    station1 = NULL;
}

/*!
 * The method connects at Track object to a Start Station and an End Station.
 *
 * @param trackID The ID number of the track.
 * @param startStation The ID number of the start station for the track.
 * @param endStation The ID number of the end station for the track.
 *
 * @return Status 1 indicates success. 0 indicates failure.
 * @TODO Insert constraints in this function, similar as the name based connectTrackToStations method
 */
int TrafficControl::connectTrackToStations(int trackID, int startStationID, int endStationID)
{
    trackList[trackID]->setStartStation(startStationID);
    trackList[trackID]->setEndStation(endStationID);
    stationList[startStationID]->addTrack(trackID);
    return 1;
}

/*!
 * The method connects at Track object to a Start Station and an End Station.
 *
 * @param trackName The name of the track.
 * @param startStationName The name of the start station for the track.
 * @param endStationName The name of the end station for the track.
 *
 * @return Status 1 indicates success. 0 indicates failure.
 */
int TrafficControl::connectTrackToStations(QString trackName, QString startStationName, QString endStationName)
{
    int foundStartStationID=-1;
    int foundEndStationID=-1;
    int foundTrackID=-1;
    if(QString::compare(startStationName, endStationName)==0) {qDebug()<<"TC:cTTS: Station names are equal";} else
    {
        foreach(Station* thisStation, stationList){
            if(QString::compare(startStationName, thisStation->getName())==0){ foundStartStationID=thisStation->getID(); }
            if(QString::compare(endStationName, thisStation->getName())==0){ foundEndStationID=thisStation->getID(); }
            if ( ( foundStartStationID != -1 ) && ( foundEndStationID != -1 ) ) break;
        }
        foreach(Track* thisTrack, trackList){
            if(QString::compare(trackName, thisTrack->getName())==0){ foundTrackID=thisTrack->getID(); break; }
        }
        if( (foundStartStationID!=-1) && ( foundEndStationID!=-1) && (foundTrackID!=-1))
        {
            if ( ( trackList[foundTrackID]->getStartStation() != -1 ) && ( trackList[foundTrackID]->getEndStation() !=-1 ) ) {cout<<"TC:CTTS, overwriting old start or end stations.\n";}
            trackList.at(foundTrackID)->setStartStation(foundStartStationID);
            trackList.at(foundTrackID)->setEndStation(foundEndStationID);
            stationList.at(foundStartStationID)->addTrack(foundTrackID);
        } else {
            qDebug()<<"TC:cTTS: Either track or station name is not found.";
            return 0;
        }
    }
    return 1;
}

/*!
 * The method is a macro that creates a simple network and adds a train to it.
 * DELETE THIS METHOD
 */
void TrafficControl::createPredefinedNetworkUI()/*Clarify the relationship between this method and the
        class TrafficControl::importPredefinedNetwork()*/
{
}


/*!
 * The method lists the tracks in the network to the console.
 * DELETE THIS METHOD
 *
 * @todo implement a foreach statement instead.
 */
void TrafficControl::listTracksInNetwork()
{
    qDebug()<<"Contents of trackList:";
    foreach(Track* thisTrack, trackList){ thisTrack->showInfo(); }
    qDebug()<<"----End of trackList---";
}

/*!
 * The method lists the trains in the network to the console.
 *
 * DELETE THIS METHOD
 *
 * @todo Implement a foreach statement instead.
 */
void TrafficControl::listTrainsInNetwork()
{
    qDebug()<<"Contents of trainList:";
    foreach(Train* thisTrain, trainList){ thisTrain->showInfo(); }
    qDebug()<<"----End of trainList----";
}

/*!
 * The method lists the stations in the network to the console.
 *
 * DELETE THIS METHOD
 *
 */
void TrafficControl::listStationsInNetwork()
{
    qDebug()<<"Contents of stationList:";
    foreach(Station* thisStation, stationList){ thisStation->showInfo(); }
    qDebug()<<"----End of stationList----";
}

/*!
 * This method commands all trains to move the amount of time defined in the stepTimeBox item in the UI.
 * @TODO: make method update all traffic elements
 */
void TrafficControl::stepTimeForNetwork()
{
    QMutexLocker locker(&mutex);
    int ddd;
    int n=ui->stepTimeBox->value();
    foreach(Train* thisTrain, trainList){
        ddd=thisTrain->move(n);
    }
    ui->stationListTableView->resizeColumnsToContents();
    ui->trackListTableView->resizeColumnsToContents();
    ui->trainListTableView->resizeColumnsToContents();
}

void TrafficControl::onRunThreadCheckBoxChanged(int newState)
{
    if(newState==2)//HARDCODED TO 2, THAT MEANS ACTIVE
    {
        trafficClock.resumeThread();
    }
    else
    {
        trafficClock.pauseThread();
    }
}

void TrafficControl::onTickIntervalChanged(int newInterval)
{
    trafficClock.setTickInterval(newInterval);
}

/*!
 * The destructor method
 * @todo Investigate what objects ad data structures shall be removed at termination of the program at termination.
 */
TrafficControl::~TrafficControl()
{

    trafficClock.disconnectThread();
    clockThread.terminate();
    while(!clockThread.isFinished()){}
    qDebug()<<"The thread is terminated. Deleting pointers.";
    foreach(Station* thisStation, stationList){ delete thisStation; thisStation=NULL;}
    foreach(Track* thisTrack, trackList){ delete thisTrack; thisTrack=NULL;}
    foreach(Train* thisTrain, trainList){ delete thisTrain; thisTrain=NULL;}
    delete trackListModel;
    delete trainListModel;
    delete stationListModel;
    //Insert code to remove objects here
    delete ui;
}
