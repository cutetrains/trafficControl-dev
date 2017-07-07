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
#include <QQuickView> //New
#include <QQuickItem> //New
#include <QQmlComponent> //New
#include <QQmlEngine> //New

#include "TrafficControl.h"
#include "ui_TrafficControl.h"
#define TRACK 1
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1

using namespace std;

TrafficControl::TrafficControl(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::TrafficControl)
{
  ui->setupUi(this);

  trafficClock.threadSetup(clockThread);// Use the old implementation of clockThread (see Traffic - R1C012_Independent_Thread)
  trafficClock.moveToThread(&clockThread);
  clockThread.start();

  connect(ui->createTrainButton, SIGNAL(clicked()), this, SLOT(addTrainToNetworkUI()));
  connect(ui->importNetworkButton, SIGNAL(clicked()), this, SLOT(importPredefinedNetwork()));
  connect(ui->timeTickButton, SIGNAL(clicked()), this, SLOT(stepTimeForNetwork()));
  connect(ui->runThreadCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onRunThreadCheckBoxChanged(int)));
  connect(ui->tickIntervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onTickIntervalChanged(int)));
  connect(&trafficClock, SIGNAL(stepTimeSignal()), this, SLOT(stepTimeForNetwork()));

  trackListModel=new TrafficDataModel(TRACK, 0);
  trainListModel=new TrafficDataModel(TRAIN, 0);
  stationListModel=new TrafficDataModel(STATION, 0);
  ui->trackListTableView->setModel(trackListModel);
  ui->trackListTableView->show();
  ui->trainListTableView->setModel(trainListModel);
  ui->trainListTableView->show();
  ui->stationListTableView->setModel(stationListModel);
  ui->stationListTableView->show();
  QUrl source("qrc:Traffic/qml/map.qml");

  ui->mapQuickWidget->setSource(source);
  handleQMLObject = ui->mapQuickWidget->rootObject();
  //  handleQMLObject->dumpObjectTree();

  /* TODO: Rename nameMainMap */
  QObject *handleQMLMainMap = handleQMLObject->findChild<QObject *>(QString("nameMainMap"));
  if (NULL == handleQMLMainMap)
  {
    qDebug()<<"ERROR  : No nameMainMap found";
  }

  /*  QObject *myStation2 = handleQMLObject->findChild<QObject *>(QString("nameMainMapCircle1"));
    if(myStation2 != NULL)
    {
        qDebug()<<"INFO   : nameMainMapCircle1 found";
        myStation2->setProperty("color","yellow");
    }
  */
}

/*!
 * The method creates a Track Object, connects it to the onDataChanged slot and
 * appends it to the trackList and the trackListModel.
 *
 * @param trackName Name of the track
 * @param trackLength Length of the track [Unit: m]
 */
void TrafficControl::addTrackToNetwork(QString trackName,
                                       int trackLength,
                                       QStringList coordinates)
{
  Track* track1 = new Track(trackName,
                            trackLength,
                            coordinates,
                            trackList,
                            trainList,
                            stationList);
  trackList.append(track1);
  trackListModel->insertRows(trackList.size(), 1 , QModelIndex());
  connect(track1,
          SIGNAL(dataChangedSignal(int, const QVariant &)),
          trackListModel,
          SLOT(onDataChanged(int, const QVariant &)));
  if(FALSE == coordinates.empty())
  {
    QVariant returnedValue;
    QMetaObject::invokeMethod(handleQMLObject,
                              "createQMLTrack",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, trackName),
                              Q_ARG(QVariant, trackLength),
                              Q_ARG(QVariant, coordinates));
    //Connect signal and slot
    connect(track1,
            SIGNAL(qmlTrackStatusSignal(QVariant ,QVariant ,QVariant)),
            handleQMLObject,
            SLOT(qmlTrackStatusSlot(QVariant, QVariant, QVariant)));
  }
  track1=NULL;
}


/*!
 * The method addTrackToNetwork creates a Train object with parameters from
 * the User Interface. It connects the train object to the onDataChanged slot
 * and appends it to the trainList and the trainListModel.
 */
void TrafficControl::addTrainToNetworkUI()
{
  qDebug()<<"WARNING: This function has intentionally been removed";
}

/*!
 * The method addTrainToNetwork creates a Train object, connects it to the
 * onDataChanged slot and appends it to the trainList and the trainListModel.
 *
 * @param nbrOfPassengers Maximum allowed number of passengers. COMMENT: This
 *        naming is confusing and should be maxNbrOfPassengers.
 */
void TrafficControl::addTrainToNetwork(QString trainName)
{
  Train* newTrain =new Train(trainName, trackList, trainList, stationList);
  connect(newTrain,
          SIGNAL(dataChangedSignal(int, const QVariant &)),
          trainListModel,
          SLOT(onDataChanged(int , const QVariant &)));
  trainList.append(newTrain);

  trainListModel->insertRows(trainList.size(), 1 , QModelIndex());
  trainList[trainList.size()-1]->load(2);/*Maybe send datachanged? 2 refers to number
                                          of passengers and is hardcoded. Change this later*/
  //Add train to QML map
  if( NULL != handleQMLObject){
    QVariant returnedValue;
    QMetaObject::invokeMethod(handleQMLObject,
                              "createQMLTrain",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, trainName));
    connect(newTrain,
            SIGNAL(qmlTrainPositionSignal(QVariant, QVariant, QVariant)),
            handleQMLObject,
            SLOT(qmlTrainPositionSlot(QVariant, QVariant, QVariant)));
  }
  newTrain = NULL;
}

/*!
 * The method addStationToNetwork creates a Train object, connects it to the
 * onDataChanged slot and appends it to the trainList and the trainListModel.
 *
 * @param stationName Name of the station
 */
void TrafficControl::addStationToNetwork(QString stationName,
                                         bool isJunction,
                                         QString stationLat,
                                         QString stationLon)
{
  Station* newStation = new Station(stationName,
                                  isJunction,
                                  stationLat,
                                  stationLon,
                                  trackList,
                                  trainList,
                                  stationList);
  connect(newStation,
          SIGNAL(dataChangedSignal(int, const QVariant &)),
          stationListModel,
          SLOT(onDataChanged(int , const QVariant &)));

  stationList.append(newStation);
  stationListModel->insertRows(stationList.size(), 1 , QModelIndex());
  stationList[stationList.size()-1]->changeNbrOfPassengers(0);
  //ADD CHECK FOR handleQMLObject!!!
  if( 0 != stationLat.compare("") && 0 != stationLon.compare("")){
    QVariant returnedValue;
    QMetaObject::invokeMethod(handleQMLObject,
                              "createQMLStation",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, stationName),
                              Q_ARG(QVariant, isJunction),
                              Q_ARG(QVariant, stationLat),
                              Q_ARG(QVariant, stationLon));
    connect(newStation,
            SIGNAL(qmlStationOccupancySignal(QVariant ,QVariant ,QVariant)),
            handleQMLObject,
            SLOT(qmlStationOccupancySlot(QVariant, QVariant, QVariant)));
  }
  newStation = NULL;
}

/*!
 * The method connects at Track object to a Start Station and an End Station.
 *
 * @param trackID The ID number of the track.
 * @param startStation The ID number of the start station for the track.
 * @param endStation The ID number of the end station for the track.
 *
 * @return Status 1 indicates success. 0 indicates failure.
 * @TODO Insert constraints in this function, similar as the name based
 *       connectTrackToStations method
 *
 * @TODO Add error detection
 */
int TrafficControl::connectTrackToStations(int trackID,
                                           int startStationID,
                                           int endStationID)
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
int TrafficControl::connectTrackToStations(QString trackName,
                                           QString startStationName,
                                           QString endStationName)
{
  int foundStartStationID = UNDEFINED;
  int foundEndStationID = UNDEFINED;
  int foundTrackID = UNDEFINED;
  if(QString::compare(startStationName, endStationName) == 0)
  {
    qDebug()<<"TC:cTTS: Station names are equal";
    return 0;
  }
  else
  {
    foreach(Station* thisStation, stationList)
    {
      if (QString::compare(startStationName, thisStation->getName()) == 0)
      {
        foundStartStationID=thisStation->getID();
      }
      if (QString::compare(endStationName, thisStation->getName()) == 0)
      {
        foundEndStationID=thisStation->getID();
      }
      if ((foundStartStationID != UNDEFINED) &&
          (foundEndStationID != UNDEFINED))
        {
          break;
        }
      }
    }

    foreach(Track* thisTrack, trackList){
      if(QString::compare(trackName, thisTrack->getName()) == 0)
      {
        foundTrackID=thisTrack->getID();
        break;
      }
    }

  if ((foundStartStationID != UNDEFINED) &&
        (foundEndStationID != UNDEFINED) &&
        (foundTrackID != UNDEFINED))
  {
    if(trackList.at(foundTrackID)->getStartStation() == UNDEFINED)
    {
      trackList.at(foundTrackID)->setStartStation(foundStartStationID);
    }
    if(trackList.at(foundTrackID)->getEndStation() == UNDEFINED)
    {
      trackList.at(foundTrackID)->setEndStation(foundEndStationID);
    }
    stationList.at(foundStartStationID)->addTrack(foundTrackID);//
  }
  else
  {
    qDebug()<<"ERROR  : TC:cTTS: Either track or station name is not found.";
    return 0;
  }
  return 1;
}



/*!
 * This method commands all trains to move the amount of time defined in the stepTimeBox item in the UI.
 * @TODO: make method update all traffic elements
 */
void TrafficControl::stepTimeForNetwork()
{
  QMutexLocker locker(&mutex);
  int response = 0;

  int n = ui->stepTimeBox->value();
  foreach(Train* thisTrain, trainList){
    response = thisTrain->move(n);
  }
  ui->stationListTableView->resizeColumnsToContents();
  ui->trackListTableView->resizeColumnsToContents();
  ui->trainListTableView->resizeColumnsToContents();
}

void TrafficControl::onRunThreadCheckBoxChanged(int newState)
{
  if(newState == 2)//HARDCODED TO 2, THAT MEANS ACTIVE
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
 * @TODO Investigate what objects ad data structures shall be removed at
 * termination of the program.
 *
 * @TODO Investigate dependencies when removing objects
 */
TrafficControl::~TrafficControl()
{
  trafficClock.disconnectThread();
  clockThread.terminate();
  while(!clockThread.isFinished()){}
  qDebug()<<"INFO   : The thread is terminated. Deleting pointers.";
  foreach(Station* thisStation, stationList){ delete thisStation; thisStation=NULL;}
  foreach(Track* thisTrack, trackList){ delete thisTrack; thisTrack=NULL;}
  foreach(Train* thisTrain, trainList){ delete thisTrain; thisTrain=NULL;}
  delete trackListModel;
  delete trainListModel;
  delete stationListModel;
  //Insert code to remove objects here
  delete ui;
}
