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

#include <QString>
#include <QDebug>
#include <QtCore>
#include "../inc/tctrain.h"
#include "../inc/trafficcontrol.h"
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
 *
 * @todo Implement a way to limit max passengers to 1.5 times nSeats.
 *
 * @todo Implement copy and assign constructor.
 */
Train::Train(QString trainName,
             QList<Track*>& trackList,
             QList<Train*>& trainList,
             QList<Station*>& stationList)
{
  //trainName = QString::fromUtf16(tn.utf16());
  name = trainName;
  passengerCapacity = 100;
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
 *
 * @return true if successful, false if unsuccessful
 */
bool Train::addStationToTravelPlan(int stationID)
{
  if (stationID < 0){
    qDebug()<<"ERROR  : Train::addTrackToTrainRoute Error: negative number specified."
            << stationID<<" : "<< thisStationList->at(stationID)->getName();
    return false;
    }
  else
  {
    travelPlanByStationID << stationID;
  }
  sendDataChangedSignal(trainID);
  return true;
}

/*!
 * This state reflects when the train is at a station, and closing doors.
 *
 * @param n Time remaining for the train in this trafficClock tick
 *
 * @return n Time remaining after operations
 */
int Train::closingState(int n)
{
  doorOpenProgress -= 20;
  if (doorOpenProgress == 0)
  {
    state = stateTable.value("WAITING");
  }
  n--;
  return n;
}

/*!
 * The method is only used when deleting the networkControl object and will
 * set the total number of trains to zero. This is done mainly in the tests
 * to ensure that the test environment is reset.
 */
void Train::destructorResetTotalNumberOfTrains()
{
  totalNbrOfTrains = 0;
}

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
 * Get the ID of the current Track object for the Train object. 
 *
 * @return currentTrack. The value -1 indicates that the Train is not at a Track (most likely on a Station).
 */
int Train::getCurrentTrack() { return currentTrack; }

/*!
 * Get the desired/ordered speed of the Train object.
 *
 * @return desiredSpeed the desired speed [m/s].
 */
int Train::getDesiredSpeed() { return desiredSpeed; }

/*!
 * The method returns the ID number for this Track object.
 *
 * @return trackID The ID number of the Track object.
 */
int Train::getID() { return trainID;}

/*!
 * Get the current location on the Train's trip plan. The method
 * getTrainRouteVector will give the entire trip plan.
 *
 * @return trainRouteIndex The position of the Train's trip plan.
 */
int Train::getIndexTravelPlanByStationID() {return nextIndexTravelPlanByStationID;}

/*!
 * Get the name of the train.
 *
 * @return name of the train.
 */
QString Train::getName(){ return name; }

/*!
 * Get the number of passengers on the train.
 *
 * @return number of passengers on the train
 */
int Train::getNbrOfPassengers(){ return nbrOfPassengers;}

/*!
 * Get the number of seats of the train.
 *
 * @return number of seats.
 */
int Train::getPassengerCapacity(){ return passengerCapacity;}

/*!
 * Get the state of the train.
 *
 * @return state of the train.
 */
int Train::getState() {return state;}

/*!
 * Get the total number of trains.
 *
 * @return total number of trains in the network.
 */
int Train::getTotalNbrOfTrains(){ return totalNbrOfTrains; }

/*!
 * Get the position on the current Track for the train in meters.
 *
 * @return n The position for the train [m] on the current track.
 */
int Train::getTrackPosition() { return positionOnTrack;}

/*!
 * Get the Train's travel plan by stations. The method getTrainRouteIndex will
 * give the position of the next track to visit.
 *
 * @return trainVectorStations The vector of the Tracks that the train will
 * use during its trip.
 */
QList<int> Train::getTravelPlan() {return travelPlanByStationID;}

/*!
 * Load some passengers to the train.
 *
 * @todo The load function is faulty. Also send a signal.
 * @param nbrOfPassengersToLoad The numer of passengers to load to the train.
 *                              This number can be negative for unloading.
 *
 * @return deniedPassengers the number of passengers that weren't allowed to enter the or leave
 *                              the train.
 */
int Train::load(int nbrOfPassengersToLoad) {
  int newNbrOfPassengers = min(passengerCapacity,
                               max(0,
                                   nbrOfPassengers + nbrOfPassengersToLoad));
  int deniedPassengers =  (nbrOfPassengersToLoad -
                           (newNbrOfPassengers - nbrOfPassengers));
  nbrOfPassengers = newNbrOfPassengers;
  return deniedPassengers;
}

/*!
 * This state reflects when the train is at a station and loading passengers.
 *
 * @param n Time remaining for the train in this trafficClock tick
 *
 * @return n remaining time
 */
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

/*!
 * Move the Train. The method will iterate until the timer is zero or the Train
 * is blocked. In that case, it will return the remaining time to move.
 *
 * @param n Time in seconds that the Train shall move
 * 
 * @return n Remaining time to move later. If the method concludes that the
 *           Train is blocked, it will return the remaining time to move.
 *           0 indicates that the move operation is complete.
 */
int Train::move(int n)
//This function must be able to move to, from and through one or several stations.
{
  QMutexLocker locker(&mutex);
  while (n > 0){//Add PASSING and EMERGENCY
    if (stateTable.value("WAITING") == state) { n = this->waitingState(n); }
    else if (stateTable.value("READY") == state) { n = this->readyState(n); }
    else if (stateTable.value("RUNNING") == state) { n = this->runningState(n); }
    else if (stateTable.value("OPENING") == state) { n = this->openingState(n); }
    else if (stateTable.value("LOADING") == state) { n = this->loadingState(n); }
    else if (stateTable.value("CLOSING") == state) { n = this->closingState(n); }
    else if (stateTable.value("STOPPED") == state) { }
    n--;
  }
  sendDataChangedSignal(trainID);//INVESTIGATE WHEN DATACHANGEDSIGNAL SHALL BE SENT!
  return n;
}

/*!
 * This state reflects when the train is at a station and opening doors.
 *
 * @param n Time remaining for the train in this trafficClock tick
 *
 * @return n Remaining time after operations
 */
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

/*!
 * This state reflects when the train ready to move.
 *
 * @param n Time remaining for the train in this trafficClock tick
 *
 * @return n Time remaining after operations
 */
int Train::readyState(int n)
{
  if (currentStation == UNDEFINED)
  {
    qDebug()<<this->getName()<<" is in READY and at a track: "<<this->getCurrentTrack()<<". TODO: implement resume running code";
    return 0;
  }
  else
  {
    if ( (nextIndexTravelPlanByStationID ==  (int) travelPlanByStationID.size() - 1  ) &&
         (travelPlanByStationID.at(nextIndexTravelPlanByStationID) == currentStation) )
    { // Train at end of travel plan
      state = stateTable.value("STOPPED");
      return 0;
    }
    else
    {
      if (UNDEFINED == nextTrack)
      { //Find next station to travel towards
        nextTrack = thisStationList->at(currentStation)
                      ->findLeavingTrackIndexToStation(//What track to next station?
                        travelPlanByStationID.at(nextIndexTravelPlanByStationID));
        if (UNDEFINED == nextTrack)
        {
          //qDebug() << "ERROR  : "<< this->getName()<<" No leaving track";
          waitingTimer=3;
          n--;
          return n;
        }
        else
        { //Wait if an available track has been found and the waiting timer is bigger than 0.
          if (waitingTimer>0){
            waitingTimer--;
            nextTrack = UNDEFINED;
          } else {
            n = this->readyToRunningState(n);
          }
          return n;
        }
      }
    }
  }
  return n;
}

/*!
 * This state reflects when the train is at a station, waiting for the proper
 * time to leave, according to time table or network.
 *
 * @param n Time remaining for the train in this trafficClock tick
 *
 * @return n Time remaining after operations
 */
int Train::readyToRunningState(int n)
{
  bool transactionSuccess = false;
  currentTrack = nextTrack;
  transactionSuccess = thisTrackList->at(currentTrack)->addTrainToTrack(trainID);
  if(transactionSuccess)
  {
    thisStationList->at(currentStation)->trainDeparture(this->getID());
    currentStation = UNDEFINED;
    nextTrack = UNDEFINED;

    state = stateTable.value("RUNNING");

    if (nextIndexTravelPlanByStationID < ((int) travelPlanByStationID.size() - 1))
    {
      nextIndexTravelPlanByStationID++;
    }
  }
  n--;
  return n;
}

/*!
 * Handles the train when it is runnong on the track.
 * @todo Handle train movement in opposite direction.
 * @param n Time in seconds that the Train shall move
 *
 * @return n Remaining time to move later. If the method concludes that the
 * Train is blocked, it will return the remaining time to move. 0 indicates
 * that the move operation is complete.
 */
int Train::runningState(int n)
{
  bool reversed = thisTrackList->at(currentTrack)->isReversedTraffic();
  int virtualPositionOnTrack = reversed ? thisTrackList->at(currentTrack)->
                                             getLength()-positionOnTrack :
                                          positionOnTrack;
  if(virtualPositionOnTrack == thisTrackList->at(currentTrack)->getLength())
  {
    n = runningToOpeningState(n);
  } else {
    desiredSpeed = (int) min((double)maxSpeed,
                             sqrt((thisTrackList->at(currentTrack)->
                                     getLength() - virtualPositionOnTrack -
                                     currentSpeed) * 2));
    desiredSpeed = max(desiredSpeed, 1);
    if (currentSpeed != desiredSpeed)//Accelerate by 1 or -1
    {
      currentSpeed = currentSpeed + (desiredSpeed - currentSpeed) /
                     abs(desiredSpeed-currentSpeed);
    }
    virtualPositionOnTrack = virtualPositionOnTrack + currentSpeed;
    positionOnTrack = reversed ? thisTrackList->at(currentTrack)->getLength() -
                                        virtualPositionOnTrack :
                                 virtualPositionOnTrack;
    this->setTrackPosition(positionOnTrack);
  }
  n--;
  return n;
}

/*!
 * This state reflects when the transition from runnng to opening state.
 *
 * @param n Time remaining for the train in this trafficClock tick
 *
 * @return n Remaining time after operation
 */
int Train::runningToOpeningState(int n)
{
  bool transactionSuccess = false;
  currentStation = thisTrackList->at(currentTrack)->isReversedTraffic()?
                     thisTrackList->at(currentTrack)->getStartStation():
                     thisTrackList->at(currentTrack)->getEndStation();
  thisStationList->at(currentStation)->trainArrival(this->getID());
  currentSpeed = 0;
  transactionSuccess = thisTrackList->at(currentTrack)->deleteTrainFromTrack(trainID);
  if(false == transactionSuccess){
    qDebug()<<"ERROR   : Train removal failed";
  }
  currentTrack = UNDEFINED;
  nextTrack = UNDEFINED;

  emit qmlTrainPositionSignal(this->getName(),
                              thisStationList->at(currentStation)->getLatitude(),
                              thisStationList->at(currentStation)->getLongitude());

  state = stateTable.value("OPENING");
  doorOpenProgress = 0;
  return n;
}

/*!
 * The method prepares a message and sends it to the Train data model. The signal
 * contains the name of the train, the current station/track, the position on the
 * track and the current speed.
 *
 * @param trainID The ID of the Train object that has information to update.
 */
void Train::sendDataChangedSignal(int trainID){//Name, Track/Station , Position, Speed
  QStringList message;
  message << name;
  if (((UNDEFINED != currentStation) && (UNDEFINED != currentTrack)) ||
      ((UNDEFINED == currentTrack) && (UNDEFINED == currentStation))){
    message<<"ERROR: Invalid position!";
  }
  if (UNDEFINED != currentStation)
  {
    message << thisStationList->at(currentStation)->getName();
  }
  else if (UNDEFINED != currentTrack)
  {
    message<<thisTrackList->at(currentTrack)->getName();
  }
  message<<QString::number(positionOnTrack);
  message<<QString::number(currentSpeed);
  message<<stateTable.key(state);
  emit dataChangedSignal(trainID, QVariant(message));
}

/*!
 * Set the current station for the Train.
 * @todo Later: reserve place in station for the train
 *
 * @param stationID The ID for the station that will be the current station for this Train.
 */
void Train::setCurrentStation(int stationID) {
  currentStation = stationID;
  thisStationList->at(stationID)->trainArrival(this->getID());
  state = stateTable.value("LOADING");

  sendDataChangedSignal(trainID);//+1?
  emit qmlTrainPositionSignal(this->getName(),
                              thisStationList->at(stationID)->getLatitude(),
                              thisStationList->at(stationID)->getLongitude());
}

/*!
 * The method sets the current Track of the Train object.
 * @TODO ADD PPOSITION
 *
 * @param trackID The ID of the Train object that has information to update.
 */
void Train::setCurrentTrack(int trackID) {
  currentTrack = trackID;
  sendDataChangedSignal(trainID);
}

/*!
 * Set the desired speed for the Train.
 *
 * @param newDesiredSpeed The desired speed for the Train [m/s].
 */
void Train::setDesiredSpeed(int newDesiredSpeed) {
  desiredSpeed = min(maxSpeed, newDesiredSpeed);
  if (currentTrack != UNDEFINED){
  int maxSpeed = thisTrackList->at(currentTrack)->getMaxAllowedSpeed();
    desiredSpeed = min(newDesiredSpeed, maxSpeed);
  }
  sendDataChangedSignal(trainID);//+1?
}

/*!
 * The method sets the current name of the Train object.
 *
 * @param name The new name of the Train object that has information to update.
 */
void Train::setName(QString trainName) {
  name = trainName;
  sendDataChangedSignal(trainID); //+1?
}

/*!
 * Set the position on the current Track for the train in meters. This is done
 * only if the train is located on a track. After completion, a signal is sent.
 *
 * @param newPosition The desired position for the train [m].
 */
void Train::setTrackPosition(int newPosition)
{
  if (currentTrack != UNDEFINED){
    positionOnTrack = max(newPosition, 0);
    positionOnTrack = min(positionOnTrack,
                          thisTrackList->at(currentTrack)->getLength());
    if(thisTrackList->at(currentTrack)->hasCoordinates())
    {
      trainCoordinates = thisTrackList->at(currentTrack)->getCoordinatesFromPosition(positionOnTrack);
      emit qmlTrainPositionSignal(this->getName(),
                                  trainCoordinates.at(0),
                                  trainCoordinates.at(1));
    }
  }
  sendDataChangedSignal(trainID);
}

/*!
 * The method shows the information of the train to the console.
 */
void Train::showInfo()
{
  qDebug() << "INFO   : "<< name <<" has " << passengerCapacity << " seats.  "
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
  foreach(int i, travelPlanByStationID)  {
    qDebug()<<"         "<<i<<": " <<thisStationList->at(i)->getName();
    if(nextIndexTravelPlanByStationID==i)
    {
      qDebug()<<"        ^ Current ^";
    }
  }
}

/*!
 * This state reflects when the train is at a station, waiting for the proper
 * time to leave, according to time table or network.
 *
 * @param n Time remaining for the train in this trafficClock tick
 *
 * @return n Remaining time after operation
 */
int Train::waitingState(int n)
{
  waitingTimer = 1;//TODO: SET THIS TO A RANDOM NUMBER
  state = stateTable.value("READY");
  return n;
}

Train::~Train()
{
}
