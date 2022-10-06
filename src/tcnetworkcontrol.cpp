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

/*! @file tcnetworkcontrol.cpp
 * @class NetworkControl
 *
 * @brief Implementation of the NetworkControl class
 *
 * The class contains the implementation of the Station object
 *
 * @author Gustaf Brännberg
 *
 * Contact: gustaf_brannberg@hotmail.com
 */

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlEngine>
#include "../inc/trafficcontrol.h"
#include "../inc/tcnetworkcontrol.h"
#define TRACK 1
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1

using namespace std;

NetworkControl::NetworkControl(TrafficDataModel &trackListModel,
                               TrafficDataModel &trainListModel,
                               TrafficDataModel &stationListModel,
                               QObject &handleQMLObject)
{
  this->trackListModel = &trackListModel;
  this->trainListModel = &trainListModel;
  this->stationListModel = &stationListModel;
  this->handleQMLObject = &handleQMLObject;

  trafficClock.threadSetup(clockThread);
  trafficClock.moveToThread(&clockThread);
  clockThread.start();

  cmdParserCurrentTrain = UNDEFINED;
  cmdParserCurrentStation = UNDEFINED;
  cmdParserCurrentTrack = UNDEFINED;
  connect(&trafficClock, SIGNAL(stepTimeSignal()), this, SLOT(stepTimeForNetwork()));
  //This is an intermediate step, relaying a signal from trafficClock via networkControl to UI
  connect(&trafficClock,
          SIGNAL(updateSimulatedTimeSignal(QString)),
          this,
          SLOT(onUpdatedSimulatedTimeSignal(QString)));
}

/*!
 * The method addStationToNetwork creates a Train object, connects it to the
 * onDataChanged slot and appends it to the trainList and the trainListModel.
 *
 * @param stationName Name of the station
 * @param isJunction TRUE if this is a junction that doesn't handle passengers
 * @param stationLat Latitude of the station.
 * @param stationLon Longitude of the station
 */
void NetworkControl::addStationToNetwork(QString stationName,
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

  stationListModel->insertRows(stationListModel->rowCount(), 1 , QModelIndex());

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
 * The creates a Track Object, connects it to the onDataChanged slot and
 * appends it to the trackList and the trackListModel.
 *
 * @param trackName Name of the track
 * @param trackLength Length of the track [Unit: m]
 */
bool NetworkControl::addTrackToNetwork(QString trackName,
                                       int trackLength,
                                       QStringList coordinates)
{
  if(trackLength <= 0) return false;

  Track* newTrack = new Track(trackName,
                              trackLength,
                              coordinates,
                              trackList,
                              trainList,
                              stationList);
  trackList.append(newTrack);
  trackListModel->insertRows(trackListModel->rowCount(), 1 , QModelIndex()); //Better to use actual size of trackListModel

  connect(newTrack,
          SIGNAL(dataChangedSignal(int, const QVariant &)),
          trackListModel,
          SLOT(onDataChanged(int, const QVariant &)));
  if(false == coordinates.empty())
  {
    QVariant returnedValue;
    QMetaObject::invokeMethod(handleQMLObject,
                              "createQMLTrack",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, trackName),
                              Q_ARG(QVariant, trackLength),
                              Q_ARG(QVariant, coordinates));
    connect(newTrack,
            SIGNAL(qmlTrackStatusSignal(QVariant, QVariant ,QVariant)),
            handleQMLObject,
            SLOT(qmlTrackStatusSlot(QVariant, QVariant, QVariant)));
  }
  newTrack = NULL;
  return true; //TODO: SHOULD THERE BE A CHECK, OR SHOULD THE FUNCTION BE VOID?
}

/*!
 * The method addTrainToNetwork creates a Train object, connects it to the
 * onDataChanged slot and appends it to the trainList and the trainListModel.
 *
 * @param nbrOfPassengers Maximum allowed number of passengers. COMMENT: This
 *        naming is confusing and should be maxNbrOfPassengers.
 * @todo: CHANGE NAME TO maxNbrOfPassengers
 */
void NetworkControl::addTrainToNetwork(QString trainName)
{
  Train* newTrain = new Train(trainName, trackList, trainList, stationList);
  connect(newTrain,
          SIGNAL(dataChangedSignal(int, const QVariant &)),
          trainListModel,
          SLOT(onDataChanged(int , const QVariant &)));
  trainList.append(newTrain);
  trainListModel->insertRows(trainListModel->rowCount(), 1 , QModelIndex());

  QVariant returnedValue;
  QMetaObject::invokeMethod(handleQMLObject,
                            "createQMLTrain",
                            Q_RETURN_ARG(QVariant, returnedValue),
                            Q_ARG(QVariant, trainName));
  connect(newTrain,
          SIGNAL(qmlTrainPositionSignal(QVariant, QVariant, QVariant)),
          handleQMLObject,
          SLOT(qmlTrainPositionSlot(QVariant, QVariant, QVariant)));

  newTrain = NULL;
}

/*!
 * The method connects at Track object to a Start Station and an End Station.
 *
 * @param trackID The ID number of the track.
 * @param startStation The ID number of the start station for the track.
 * @param endStation The ID number of the end station for the track.
 *
 * @return Status 1 indicates success. 0 indicates failure.
 * @todo Add error detection
 */
bool NetworkControl::connectTrackToStations(int trackID,
                                            int startStationID,
                                            int endStationID)
{
  if(trackID != UNDEFINED && trackID < trackList.length() &&
     startStationID != UNDEFINED && startStationID < stationList.length() &&
     endStationID != UNDEFINED && endStationID < stationList.length() &&
     startStationID != endStationID){
    if(trackList.at(trackID)->getStartStation() == UNDEFINED)
    {
      trackList.at(trackID)->setStartStation(startStationID);
    }
    if(trackList.at(trackID)->getEndStation() == UNDEFINED)
    {
      trackList.at(trackID)->setEndStation(endStationID);
    }
    stationList[startStationID]->addTrack(trackID);
    return true;
  } else {
    qDebug()<<"ERROR  : TC:cTTS: Either track or station name is not found.";
    return false;
  }
}

/*!
 * The method finds the IDs for the track and station and invokes the connect
 * method that is based on IDs.
 *
 * @param trackName The name of the track.
 * @param startStationName The name of the start station for the track.
 * @param endStationName The name of the end station for the track.
 *
 * @return Status 1 indicates success. 0 indicates failure.
 *
 * @todo Add error handling
 * @todo Add separate function for searching stationList/trackList/trainList
 *       to get an ID match to a specific name
 */
bool NetworkControl::connectTrackToStationsByName(QString trackName,
                                                  QString startStationName,
                                                  QString endStationName)
{
  int foundStartStationID = UNDEFINED;
  int foundEndStationID = UNDEFINED;
  int foundTrackID = UNDEFINED;

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

  foreach(Track* thisTrack, trackList){
    if(QString::compare(trackName, thisTrack->getName()) == 0)
    {
      foundTrackID=thisTrack->getID();
      break;
    }
  }
  return this->connectTrackToStations(foundTrackID, foundStartStationID, foundEndStationID);
}


/*!
 * The method changes the interval between each tick, depending on the
 * interval parameter.
 *
 * @param fastForwardSpeed The new speed of the thread, compared with the clock time.
 */
void NetworkControl::onFastForwardSpeedChanged(double newFastForwardSpeed)
{
  int newInterval = newFastForwardSpeed=0 ? 1000 : (int) 1000/newFastForwardSpeed;
  trafficClock.setTickInterval(newInterval);
}

/*!
 * This method relays the simulated time to the user interface.
 *
 * @param simulatedTime the simulated time in the network.
 */
void NetworkControl::onUpdatedSimulatedTimeSignal(QString simulatedTime)
{
  emit updateSimulatedTimeSignalLabel(simulatedTime);
}

/*!
 * The method resumes/pauses the trafficClock thread, depending on the new
 * state of the thread.
 *
 * @param simulationIsPaused Bool to tell if the simulation shall be paused.
 */
void NetworkControl::setSimulationPaused(bool isPaused)
{
  isPaused ? trafficClock.pauseThread() : trafficClock.resumeThread();
}

/*!
 * This method commands all trains to move the amount of time defined in the stepTimeBox item in the UI.
 * @todo: Make method update all traffic elements, such as TRACK and STATION
 *
 * @param n number of seconds to step
 */
void NetworkControl::stepTimeForNetwork(int n)
{
  int calculationTime;
  QMutexLocker locker(&mutex);
  startTime = QTime::currentTime();
  trafficClock.incrementSimulatedTime(n);
  /* ASSERTS TO VERIFY THAT TRAFFIC ELEMENTS HAS NEVER BEEN DELETED */
  bool result = false;
  result = trainList.length() == 0 ? true :
                     trainList.length() == trainList.last()->getID() + 1;
  Q_ASSERT_X(result,
             Q_FUNC_INFO,
             "Train ID/list mismatch. Train objects mustn't be deleted!");

  result = trackList.length() == 0 ? true :
                     trackList.length() == trackList.last()->getID() + 1;
  Q_ASSERT_X(result,
             Q_FUNC_INFO,
             "Track ID/list mismatch. Track objects mustn't be deleted!");
  result = stationList.length() == 0 ? true :
                       stationList.length() == stationList.last()->getID() + 1;
  Q_ASSERT_X(result,
             Q_FUNC_INFO,
             "Station ID/list mismatch. Station objects mustn't be deleted!");

  for(int iii=0;iii<n;iii++)
  {
    foreach(Train* thisTrain, trainList){
      thisTrain->move(1);
    }
  }

  calculationTime = startTime.msecsTo(QTime::currentTime());
  emit updateCalculationLoad((int) calculationTime/n);
}

/*!
 * This method commands all trains to move one unit of time.
 */
void NetworkControl::stepTimeForNetwork(){
  stepTimeForNetwork(1);
}

/*!
 * The destructor method
 * @todo Investigate what objects ad data structures shall be removed at
 * termination of the program.
 * @todo Check if the lists are empty
 * @todo Investigate dependencies when removing objects
 */
NetworkControl::~NetworkControl()
{
  trafficClock.disconnectThread();
  clockThread.terminate();
  while(!clockThread.isFinished()){}
  foreach(Station* thisStation, stationList){
    thisStation->destructorResetTotalNumberOfStations();
    delete thisStation;
    thisStation=NULL;}
  foreach(Track* thisTrack, trackList){
    thisTrack->destructorResetTotalNumberOfTracks();
    delete thisTrack;
    thisTrack=NULL;
  }
  foreach(Train* thisTrain, trainList){
    thisTrain->destructorResetTotalNumberOfTrains();
    delete thisTrain;
    thisTrain=NULL;}
 }
