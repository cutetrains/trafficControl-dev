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
#include "../inc/trafficControl.h"
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
  if(&handleQMLObject == NULL)
  {
    qDebug()<<"NC: trackListModel is a null pointer";
  }

  trafficClock.threadSetup(clockThread);
  trafficClock.moveToThread(&clockThread);
  clockThread.start();
  cmdParserCurrentTrain = UNDEFINED;
  cmdParserCurrentStation = UNDEFINED;
  cmdParserCurrentTrack = UNDEFINED;

  connect(&trafficClock, SIGNAL(stepTimeSignal()), this, SLOT(stepTimeForNetwork()));
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
  if(trackLength <= 0) { return false; }

  Track* newTrack = new Track(trackName,
                              trackLength,
                              coordinates,
                              trackList,
                              trainList,
                              stationList);
  trackList.append(newTrack);
  trackListModel->insertRows(trackList.size(), 1 , QModelIndex());
  connect(newTrack,
          SIGNAL(dataChangedSignal(int, const QVariant &)),
          trackListModel,
          SLOT(onDataChanged(int, const QVariant &)));
  if(FALSE == coordinates.empty() &&
     &handleQMLObject != NULL)
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
  return true;
}

/*!
 * The method addTrainToNetwork creates a Train object, connects it to the
 * onDataChanged slot and appends it to the trainList and the trainListModel.
 *
 * @param nbrOfPassengers Maximum allowed number of passengers. COMMENT: This
 *        naming is confusing and should be maxNbrOfPassengers.
 */
void NetworkControl::addTrainToNetwork(QString trainName)
{
  Train* newTrain =new Train(trainName, trackList, trainList, stationList);
  connect(newTrain,
          SIGNAL(dataChangedSignal(int, const QVariant &)),
          trainListModel,
          SLOT(onDataChanged(int , const QVariant &)));
  trainList.append(newTrain);
  trainListModel->insertRows(trainList.size(), 1 , QModelIndex());

  if( NULL != &handleQMLObject){
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
  stationListModel->insertRows(stationList.size(), 1 , QModelIndex());

  if( 0 != stationLat.compare("") &&
      0 != stationLon.compare("") &&
      NULL != &handleQMLObject){
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
 * The method resumes/pauses the trafficClock thread, depending on the new
 * state of the thread.
 *
 * @param newState The new state of the thread.
 */
void NetworkControl::onRunThreadCheckBoxChanged(int newState)
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

/*!
 * The method changes the interval between each tick, depending on the
 * interval parameter.
 *
 * @param newInterval The new interval of the thread.
 */
void NetworkControl::onFastForwardSpeedChanged(double newFastForwardSpeed)
{
  int newInterval = newFastForwardSpeed=0? 1000: (int) 1000/newFastForwardSpeed;
  trafficClock.setTickInterval(newInterval);

}

/*!
 * This method commands all trains to move the amount of time defined in the stepTimeBox item in the UI.
 * @TODO: make method update all traffic elements, such as TRACK and STATION
 */
void NetworkControl::stepTimeForNetwork(int n)
{
  QMutexLocker locker(&mutex);
  int response = 0;

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
      response = thisTrain->move(1);
    }
  }
}

void NetworkControl::stepTimeForNetwork(){ stepTimeForNetwork(1);}

/*!
 * The destructor method
 * @TODO Investigate what objects ad data structures shall be removed at
 * termination of the program.
 *
 * @TODO Investigate dependencies when removing objects
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
