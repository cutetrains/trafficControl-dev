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

#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <QString>
#include <QDebug>
#include <QtCore>
#include "tctrain.h"
#include "trafficcontrol.h"
#define UNDEFINED -1
using namespace std;

int Train::totalNbrOfTrains=0; 

/*!
 * The constructor for a Train object.
 *
 * @param cap capacity for the train
 * @param trackList is a pointer to the trackList
 * @param trainList is a pointer to the trackList
 * @param stationList is a pointer to the stationList
 * @todo Implement a way to limit max passengers to 1.5 times nSeats.
 */
Train::Train(QString tn,
             QList<Track*>& trackList,
             QList<Train*>& trainList,
             QList<Station*>& stationList)
{
  trainName = QString::fromUtf16(tn.utf16());
  nbrOfSeats = 100;
  positionOnTrack = 0;
  nbrOfPassengers = 0;
  desiredSpeed = 0;
  currentSpeed = 0;
  currentStation = UNDEFINED;
  currentTrack = UNDEFINED;
  nextTrack  = UNDEFINED;

  stateTable.insert("WAITING", 0);  //More stations to visit
  stateTable.insert("READY", 1);    //Train is ready to start immediately
  stateTable.insert("RUNNING", 2);  //Train is running
  stateTable.insert("PASSING", 3);  //Train is running an will pass a station/junction without stopping
  stateTable.insert("OPENING", 4);  //Doors are opening
  stateTable.insert("LOADING", 5);  //Doors are open, loading passengers
  stateTable.insert("CLOSING", 6);  //Doors are closing
  stateTable.insert("STOPPED", 7);  //No more stations to visit or emergency brake
  stateTable.insert("EMERGENCY",8); //Train must stop immediately or at next station if in tunnel, and open doors

  trainID = totalNbrOfTrains;/*!<The trainID will be correspond to the order of created Train objects. */
  totalNbrOfTrains++;
  nextIndexTravelPlanByStationID = 0;
  maxSpeed = 40;//Replace by constant DEFAULT_MAX_SPEED
  thisTrackList = &trackList;
  thisTrainList = &trainList;
  thisStationList = &stationList;
  passing = false;
}

/*!
 * Adds a Track ID to the trainRoute in order to define the route for the train.
 *
 * @todo Integrity check to make sure that the track is valid.
 * @todo investigate if it is better to use a list of stations instead.
 * 
 * @param trackID the ID number of the track to add.
 */
int Train::addStationToTravelPlan(int stationID)
{
  if (stationID < 0){
    qDebug()<<"ERROR  : Train::addTrackToTrainRoute Error: negative number specified."
            << stationID<<" : "<< thisStationList->at(stationID)->getName();
    return 0;
    }
  else
  {
    qDebug()<<"INFO   : Adding station to trainroute" << stationID<<" : "
            << thisStationList->at(stationID)->getName();
    travelPlanByStationID.push_back(stationID);
  }
  sendDataChangedSignal(trainID);
  showInfo();
  return 1;
}

/*!
 * Get the ID of the current Track object for the Train object. 
 *
 * @return currentTrack. The value -1 indicates that the Train is not at a Track (most likely on a Station).
 */
int Train::getCurrentTrack() { return currentTrack; }

/*!
 * Get the actual speed of the Train object.
 *
 * @return currentSpeed the current speed [m/s].
 */
int Train::getCurrentSpeed() { return currentSpeed; }

/*!
 * Get the ID of the current Station object for the Train object.
 *
 * @return currentStation. The value -1 indicates that the Train is not at a Station (most likely on a Track).
 */
int Train::getCurrentStation() { return currentStation; }

/*!
 * Get the desired/ordered speed of the Train object.
 *
 * @return desiredSpeed the desired speed [m/s].
 */
int Train::getDesiredSpeed() { return desiredSpeed; }

/*!
 * Get the current location on the Train's trip plan. The method getTrainRouteVector will give the entire trip plan.
 *
 * @return trainRouteIndex The position of the Train's trip plan.
 */
int Train::getIndexTravelPlanByStationID() {return nextIndexTravelPlanByStationID;}

/*!
 * Get the name of the train.
 *
 * @return name of the train.
 */
QString Train::getName(){ return trainName; }

/*!
 * Get the Train's trip plan by stations. The method getTrainRouteIndex will give the position of the next track to visit.
 *
 * @return trainVectorStations The vector of the Tracks that the train will use during its trip.
 */
vector<int> Train::getTravelPlan() {return travelPlanByStationID;}

/*!
 * Move the Train. The method will iterate until the timer is zero or the Train is blocked. In that case, it will return the remaining time to move.
 * @todo TOTAL REFACTORING NEEDED
 * @param n Time in seconds that the Train shall move
 * 
 * @return n Remaining time to move later. If the method concludes that the Train is blocked, it will return the remaining time to move. 0 indicates that the move operation is complete.
 */
int Train::move(int n)
//This function must be able to move to, from and through one or several stations.
{
  QMutexLocker locker(&mutex);
  while (n > 0){//Add PASSING and
    if (stateTable.value("WAITING") == state) { n = this->waitingState(n); }
    else if (stateTable.value("READY") == state) { n = this->readyState(n); }
    else if (stateTable.value("RUNNING") == state) { n = this->runningState(n); }
    else if(stateTable.value("OPENING") == state) { n = this->openingState(n); }
    else if(stateTable.value("LOADING") == state) { n = this->loadingState(n); }
    else if(stateTable.value("CLOSING") == state) { n = this->closingState(n); }
    else if(stateTable.value("STOPPED") == state) { }
    n--;//REMOVE!!!
  }
  sendDataChangedSignal(trainID);//INVESTIGATE WHEN DATACHANGEDSIGNAL SHALL BE SENT!
  return n;
}

int Train::waitingState(int n)
{
  state = stateTable.value("READY");
  return n;
}

int Train::readyState(int n)
{
  if (currentStation != UNDEFINED) //Train at station.
  {
    if (nextIndexTravelPlanByStationID >= ((int) travelPlanByStationID.size() - 1))
    {
      if (travelPlanByStationID.at(nextIndexTravelPlanByStationID) == currentStation)
      {
        qDebug()<<"INFO   : "<< trainName <<" shall stop. nextIndex: "<< nextIndexTravelPlanByStationID<<" tps: "
                << (int) travelPlanByStationID.size() <<" CurrentStation: "<<currentStation<< "TP points at:"
                <<travelPlanByStationID.at(nextIndexTravelPlanByStationID);
        state = stateTable.value("STOPPED");
        return 0;
      }
    }
    else
    {
      if (UNDEFINED == nextTrack)//Next track not defined
      {
        //THIS SHOULD BE REPLACED BY A FUNCTION OR METHOD THAT SEARCHES FOR TRACK
        qDebug()<<"TRAIN  : " << this->getName() <<" in "
                <<thisStationList->at(currentStation)->getName()<< "and is searching for next track.";
        nextTrack = thisStationList->at(currentStation)
                      ->findLeavingTrackIndexToStation(//What track to next station?
                        travelPlanByStationID.at(nextIndexTravelPlanByStationID));//Next station in travelplan
        qDebug()<<"TRAIN  :"<< this->getName()<<" has found track "<< thisTrackList->at(nextTrack)->getName();
        if (UNDEFINED == nextTrack)//Still no suitable track found - IS THIS STATE POSSIBLE?
        {
          if (travelPlanByStationID.at(nextIndexTravelPlanByStationID) == currentStation)
          {
            state = stateTable.value("STOPPED");
            return 0;
          } else {
            qDebug() << "ERROR  : Train:Move No track from "
                     << thisStationList->at(currentStation)->getName()
                     << " to " << thisStationList->at(travelPlanByStationID
                                                     .at(nextIndexTravelPlanByStationID))
                                                     ->getName();
            this->showInfo();
            state = stateTable.value("STOPPED");
            return 0;
          }
        }
      }
    }

    if (true == thisStationList->at(currentStation)->checkIfTrackLeavesStation(nextTrack))
    {//Train is at a station and next track is leaving the station.
      currentTrack = nextTrack;
      currentStation = UNDEFINED;//Remove this train from station.
      nextTrack = UNDEFINED;
      positionOnTrack = 0;//TODO: Check what happens when train is traveling in opposite deirection!
      state = stateTable.value("RUNNING");
      if (nextIndexTravelPlanByStationID < ((int) travelPlanByStationID.size() - 1))
      {
        nextIndexTravelPlanByStationID++;
        this->showInfo();
      } else
      {
        qDebug()<<"INFO   : "<< trainName << " has reached end of travelplan. tp_s : "<< (int) travelPlanByStationID.size()
          <<" pos: "<<nextIndexTravelPlanByStationID;
      }//target next station
    } else {//Try to find leaving track to nextStation
      nextTrack = thisStationList->at(currentStation)
                                 ->findLeavingTrackIndexToStation(travelPlanByStationID.at
                                                                 (nextIndexTravelPlanByStationID));
      if(UNDEFINED == nextTrack)
      {
        qDebug() << "ERROR  : Train:Move! No track from " << thisStationList->at(currentStation)->getName()
                 << " to "<<thisStationList->at(travelPlanByStationID
                                                        .at(nextIndexTravelPlanByStationID))
                                                        ->getName();
        this->showInfo();
        state = stateTable.value("STOPPED");
      }
      sendDataChangedSignal(trainID);
      return n;
    }
  }
  n--;
  return n;
}

/*!
 * Handles the train when it is runnong on the track.
 * @todo Handle train movement in ipposite direction.
 * @param n Time in seconds that the Train shall move
 *
 * @return n Remaining time to move later. If the method concludes that the
 * Train is blocked, it will return the remaining time to move. 0 indicates
 * that the move operation is complete.
 */
int Train::runningState(int n)
{
  if(positionOnTrack == thisTrackList->at(currentTrack)->getLength())
  {
    currentStation = thisTrackList->at(currentTrack)->getEndStation();
    currentSpeed = 0;
    currentTrack = UNDEFINED;
    state = stateTable.value("OPENING");
    doorOpenProgress = 0;
  } else {
    //qDebug()<<"Tr:M: On a track, still some distance to the end of track.";
    desiredSpeed = (int) min((double)maxSpeed,
                             sqrt((thisTrackList->at(currentTrack)->
                                     getLength() - positionOnTrack -
                                     currentSpeed) * 2));
    desiredSpeed=max(desiredSpeed, 1);
    if (currentSpeed != desiredSpeed)
    {
      currentSpeed = currentSpeed + (desiredSpeed - currentSpeed) /
                     abs(desiredSpeed-currentSpeed);
    }
    this->setTrackPosition(positionOnTrack+currentSpeed);
  }
  n--;

  return n;
}

int Train::openingState(int n)
{
  //TODO Avoid magic numbers
  doorOpenProgress += 20;
  if (doorOpenProgress== 100)
  {
    state = stateTable.value("LOADING");
  }
  n--;
  return n;
}

int Train::loadingState(int n)
{
  //Train shall continue loading passengers until no more passengers are on station
  if (0 == thisStationList->at(currentStation)->getNbrOfWaitingPassengers())
  {
    state = stateTable.value("CLOSING");
    doorOpenProgress = 100;
    n--;
  }

  return n;
}

int Train::closingState(int n)
{
  doorOpenProgress -= 20;
  if (doorOpenProgress==0)
  {
    state = stateTable.value("WAITING");
  }
  n--;
  return n;
}

/*!
 * Set the current station for the Train. 
 *
 * @param stationID The ID for the station that will be the current station for this Train.
 */
void Train::setCurrentStation(int stationID) {
  currentStation = stationID;
  state = stateTable.value("LOADING");

  sendDataChangedSignal(trainID);//+1?
}//Later: reserve place in station for the train

/*!
 * Set the desired speed for the Train. 
 *
 * @param n The desired speed for the Train [m/s].
 */
void Train::setDesiredSpeed(int n) { 
  desiredSpeed = min(maxSpeed, n);
  if (currentTrack != UNDEFINED){
  int maxSpeed = thisTrackList->at(currentTrack)->getMaxAllowedSpeed();
    desiredSpeed = min(n, maxSpeed);
  }
  sendDataChangedSignal(trainID);//+1?
}

/*!
 * The method returns the ID number for this Track object.
 *
 * @return trackID The ID number of the Track object.
 */
int Train::getID() { return trainID;}

/*!
 * Set the position on the current Track for the train in meters. This is done
 * only if the train is located on a track. After completion, a signal is sent.
 *
 * @param n The desired position for the train [m].
 */
void Train::setTrackPosition(int n)
{
  if (currentTrack != UNDEFINED){
    positionOnTrack = max(n, 0);
    positionOnTrack = min(positionOnTrack,
                          thisTrackList->at(currentTrack)->getLength());
  }
  sendDataChangedSignal(trainID);//+1?
}

/*!
 * Get the position on the current Track for the train in meters. 
 *
 * @return n The position for the train [m] on the current track.
 */
int Train::getTrackPosition() { return positionOnTrack;}

/*!
 * Load some passengers to the train.
 * 
 * @todo The load function is faulty. Also send a signal.
 * @param n The numer of passengers to load to the train.
 */
void Train::load( int n) {n++;}	

/*!
 * The method prepares a message and sends it to the Train data model. The signal
 * contains the name of the train, the current station/track, the position on the
 * track and the current speed.
 *
 * @param trainID The ID of the Train object that has information to update.
 */
void Train::sendDataChangedSignal(int trainID){//Name, Track/Station , Position, Speed
  QStringList message;
  message << trainName;
  if (((UNDEFINED != currentStation) && (UNDEFINED != currentTrack)) ||
      ((UNDEFINED == currentTrack) && (UNDEFINED == currentStation))){
    message<<"ERROR: Invalid position!";
  }
  if (UNDEFINED != currentStation)
  {
    message << "S[" + QString::number(currentStation) + "] " +
               thisStationList->at(currentStation)->getName();
  }
  else if (UNDEFINED != currentTrack)
  {
    message<<"T["+QString::number(currentTrack)+"] "+thisTrackList->at(currentTrack)->getName();
  }
  message<<QString::number(positionOnTrack);
  message<<QString::number(currentSpeed);
  message<<stateTable.key(state);
  emit dataChangedSignal(trainID, QVariant(message));
}

/*!
 * The method sets the current Track of the Train object.
 *
 * @param trackID The ID of the Train object that has information to update.
 */
void Train::setCurrentTrack(int trackID) {
  currentTrack = trackID;
  sendDataChangedSignal(trainID); //+1?
}

/*!
 * The method sets the current name of the Train object.
 *
 * @param name The new name of the Train object that has information to update.
 */
void Train::setName(QString const& tn) {
  trainName = QString::fromUtf16(tn.utf16());
  sendDataChangedSignal(trainID); //+1?
}

/*!
 * The method shows the information of the train to the console.
 */
void Train::showInfo()
{
  int iii=0;
  vector<int>::iterator it;
  qDebug() << "INFO   : "<< trainName <<" has " << nbrOfSeats << " seats.  "
           << nbrOfPassengers << " are taken. ";
  if(UNDEFINED != currentTrack)
  {
    qDebug() << "       : CurrentTrack is not null";
    qDebug() << "       : Speed: " << currentSpeed << " m/s, track:"
             << thisTrackList->at(currentTrack)->getName()
             << " at position " << positionOnTrack << ". ";
  }
  if(UNDEFINED != currentStation)
  {
    qDebug() << "       : Train is at station "
             << thisStationList->at(currentStation)->getName();
  }
  qDebug() << "       : TrainRoute, index: " << nextIndexTravelPlanByStationID
           << " : ";
  for (it = travelPlanByStationID.begin(); it != travelPlanByStationID.end(); ++it )
  {
    qDebug()<<"         "<<*it<<": " <<thisStationList->at(*it)->getName();
    if(nextIndexTravelPlanByStationID==iii)
    {
      qDebug()<<"        ^ Current ^";
    }
    iii++;
  }
}

Train::~Train()
{
}
