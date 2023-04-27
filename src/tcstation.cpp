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

/*! @file tcstation.cpp
 * @class Station
 *
 * @brief Implementation of the Station class
 *
 * The class contains the implementation of the Station object
 *
 * @author Gustaf Brännberg
 *
 * Contact: gustaf_brannberg@hotmail.com
 */

#include <QDebug>
#include "../inc/tcstation.h"
#include "../inc/trafficcontrol.h"
#define UNDEFINED -1

using namespace std;
int Station::totalNbrOfStations = 0;

/*!
 * The constructor for a Station object.
 *
 * @param cn Name of the station
 * @param isJunction is true if the station is a junction
 * @param latitude
 * @param longitude
 * @param trackList is a pointer to the trackList
 * @param trainList is a pointer to the trackList
 * @param stationList is a pointer to the stationList
 *
 * @todo Rename cn to a more readable name
 * @todo numberOfPlatforms is hardcoded to 2,  should be station-dependent.
 * @todo emit signal at initialisation so that QML has updated info
 */
Station::Station(QString stationName,
                 bool isJunction,
                 QString latitude,
                 QString longitude,
                 QList<Track*>& trackList,
                 QList<Train*>& trainList,
                 QList<Station*>& stationList)
{
  name = stationName;
  waitingPassengers = 0;
  stationIsJunction = isJunction;
  numberOfPlatforms = 2;

  bool isOk;
  float tempLatitude;
  float tempLongitude;
  tempLatitude = latitude.toFloat(&isOk);
  if (isOk) {
    tempLongitude = longitude.toFloat(&isOk);
  }
  if(isOk) {  //Verify that coordinates are not too out of scope
    isOk = (qFabs(tempLatitude) < 90 && qFabs(tempLongitude) < 180);
  }
  hasValidCoordinates = isOk;
  if (isOk) thisCoordinate<<tempLatitude<<tempLongitude;

  stationID = totalNbrOfStations;
  totalNbrOfStations++;
  thisTrackList = &trackList;
  thisTrainList = &trainList;
  thisStationList = &stationList;
  //sendDataChangedSignal(stationID);
}

/*!
 * The method adds a track, trackID, to the vector of leaving tracks for the
 * station. Currently, only leaving tracks are recorded in the object, not
 * incoming tracks.
 *
 * @param trackID The ID of the track that shall be added to the list of
 *                leaving tracks for the station.
 * @todo Check if track exists already and check if track is already leaving
 *       this or other stations, using checkIfTrackLeavesStation for all
 *       stations.
 * @todo Check if track is already connected to the station
 */
bool Station::addTrack(int trackID) {
  if (trackID<= UNDEFINED ||
      trackID > thisTrackList->length() - 1 ||
      this->leavingTrackList.contains(trackID)){
    qDebug()<<"ERROR  : Station::addTrack" << name
            <<" Error: bad number specified: "<<trackID<<"LT: "<<leavingTrackList;
    return false;
  }
  else
  {
    leavingTrackList << trackID;
    sendDataChangedSignal(stationID);
    return true;
  }
}

/*!
 * The adds a number of passengers to the current number of waiting passengers
 * and notifies the model.
 *
 * @param nbrPassengersToAdd The number of passengers that shall be added to
 *        the station. A negative number means that some passengers are leaving the station.
 *
 * @return result
 */
bool Station::changeNbrOfPassengers(int nbrPassengersToAdd) {
  if(this->isJunction())
  {
    qDebug()<<"ERROR  : Tried to change number of passengers for Junction: "<< this->getName();
    return false;
  }
  else
  {
    bool result = (nbrPassengersToAdd + waitingPassengers < 0) ? false: true;
    waitingPassengers = max((waitingPassengers + nbrPassengersToAdd), 0);
    sendDataChangedSignal(stationID);
    return result;
  }
}

/*!
 * The method checks if track trackID is listed in the leavingTrackList.
 *
 * @param trackID The ID of the track that shall be added to the list of
 *                leaving tracks for the station.
 *
 * @return bool true if the track is listed as leaving for this station, false
 *              otherwise.
 */
bool Station::checkIfTrackLeavesStation(int trackID)
{
  bool foundTrack = false;
  foreach(int i, leavingTrackList)  {
    if(trackID == i)
    {
      foundTrack = true;
    }
  }
  return foundTrack;
}

/*!
 * The method is only used when deleting the networkControl object and will
 * set the total number ofstations to zero. This is done mainly in the tests
 * to ensure that the test environment is reset.
 */
void Station::destructorResetTotalNumberOfStations() { totalNbrOfStations = 0; }

/*!
 * The method finds the track index in leavingTrackList to the adjacent
 * station stationID.
 *
 * If no tracks in preferred direction are found, the method shall search for
 * tracks in reversed direction. If no tracks are found, or the station isn't
 * adjacent, the method will return -1.
 *
 * @param stationID The ID of the station to reach.
 *
 * @return trackID The ID number the station.
 * @todo consider whether track is locked or not
 */
int Station::findLeavingTrackIndexToStation(int targetStationID)
{
  if(this->getID() != targetStationID) {
    foreach(int i, leavingTrackList) {
      if((thisTrackList->at(i)->getEndStation()) == targetStationID) {
        if( !thisTrackList->at(i)->isLockedDownStream() ) {
          return i;
        }
      }
    }
    foreach(int i, leavingTrackList) {
      if((thisTrackList->at(i)->getStartStation()) == targetStationID) {
        if( !thisTrackList->at(i)->isLockedUpStream() ) {
          return i;
        }
      }
    }
  }
  return UNDEFINED;
}

/*!
 * The method returns ID number of the station
 *
 * @return stationID The ID number the station.
 */
int Station::getID() { return stationID; }

/*!
 * The method returns the name of the station.
 *
 * @return name Name of the station.
 */
QString Station::getName() { return name; }

/*!
 * The method returns the number of waiting passengers at the current station
 * @todo When passengers have different destinations, a more advanced method is
 *       needed. That needs to list passengers according to their desired
 *       destinations.
 *
 * @return waitingPassengers The number of waiting passengers.
 */
int Station::getNbrOfWaitingPassengers() { return waitingPassengers; }

/*!
 * The method returns the latitude of the station
 *
 * @return latitude The latitude of the station.
 */
float Station::getLatitude() {
  return (hasValidCoordinates ? thisCoordinate.at(0): 0);
}

/*!
 * The method returns the leavingTrackList of the station
 *
 * @return leavingTrackList The list of the leaving tracks from the station
 */
QList<int> Station::getLeavingTrackList() { return leavingTrackList;}


/*!
 * The method returns the longitude of the station
 *
 * @return latitude The longitude of the station.
 */
float Station::getLongitude() {
  return (hasValidCoordinates ? thisCoordinate.at(1): 0);
}

/*!
 * The method returns the number of platforms.
 *
 * @return nbrOfPlatforms The number of platfoms/tracks on the station.
 */
int Station::getNbrOfPlatforms(){ return numberOfPlatforms;}

/*!
 * The method returns the number of platforms.
 *
 * @return totalNbrOfStations The number of stations in the entire network.
 */
int Station::getTotalNbrOfStations(){ return totalNbrOfStations; }

/*!
 * The method returns the trainList of the station
 *
 * @return trainList The list of present train at station
 */
QList<int> Station::getTrainList() { return trainsAtStationList;}

/*!
 * The method returns whether station has coordinates or not
 *
 * @return bool True if coordinates are present, False if not
 */
bool Station::hasCoordinates() { return hasValidCoordinates; }

/*!
 * The method returns whether station is Junction or not
 *
 * @return bool True if station is Junction, False if not
 */
bool Station::isJunction() { return stationIsJunction; }

/*!
 * The method prepares a message and sends it to the Station data model. The
 * signal contains the name of the station and the number of waiting passengers.
 * @todo Check how Doxygen handles signals/slots in the documentation.
 *
 * @param stationID The ID of this Station object.
 */
void Station::sendDataChangedSignal(int stationID){
  QStringList message;
  message<<name;
  message<<QString::number(waitingPassengers);
  emit dataChangedSignal(stationID, QVariant(message));
}

/*!
 * The method specifies the number of platforms. The default number of
 * platforms is 2.
 *
 * @param nbrOfPlatforms The new number of platfoms/tracks on the station.
 *
 * @return TRUE if successful
 */
bool Station::setNbrOfPlatforms(int nbrOfPlatforms){
  if(nbrOfPlatforms > 0){
    numberOfPlatforms = nbrOfPlatforms;
    return true;
  }
  else {
    return false;
  }
}

/*!
 * The method shows information about the Station object (Name, number of
 * waiting passengers, number of leaving tracks) to the console.
 * @todo Check if a foreach statement can simplify the code.
 */
void Station::showInfo() {
  qInfo() << name << " (" << stationID << ") has " << waitingPassengers
           << " waiting passengers. ";
  qInfo() << "INFO   : Leaving tracks: ";
  foreach(int i, leavingTrackList)  { qInfo() <<"        "<<i; }
  qInfo() << "INFO : thisTrackList: Size is " << thisTrackList->size()
           << "thisStationList: Size is " << thisStationList->size()
           << " thisTrainList: Size is " << thisTrainList->size() << "\n";
}

/*!
 * The method adds a train to the station, if there is room for it. It also
 * emits a signal to QML.
 *
 * @param trainID The ID of the train.
 *
 * @return TRUE if successful
 */
bool Station::trainArrival(int trainID)
{
  //bool result = false; Used later
  if(trainsAtStationList.length() >= numberOfPlatforms)
  {
    qDebug()<<"ERROR  : TOO MANY TRAINS!" <<thisTrainList->at(trainID)->getName()<<
              " arrived to the full station" << this->getName();
    return false;
  }
  else
  {
    if(trainsAtStationList.contains(trainID))
    {
      //exit(1); // OK
      //exit(0) //exits, but wrong exit code
      qFatal("test123");// OK, but different strings accepted with messagehandler
      //Q_ASSERT_X(result,
      //           Q_FUNC_INFO,
      //           "Station::Arrival, the train is already at the station!"); //NOK
      //Q_ASSERT(result); //NOK
      //assert(result); //NOK
      //FAIL()<<"This shouldn't happen!"; //NOK
      return false;
    } else {
      if(trainID < 0 ||
         trainID > thisTrainList->length() - 1 )
      {
        return false;
      }
    }
    trainsAtStationList<<trainID;
    emit qmlStationOccupancySignal(this->getName(), trainsAtStationList.length(), numberOfPlatforms);
    return true;
  }
}

/*!
 * The method removes a train to the list of trains currently in the station and
 * emits a signal to QML.
 *
 * @param trainID The ID of the train.
 *
 * @return true/false depending on whether the removal was successful
 */
bool Station::trainDeparture(int trainID)
{
  bool removalSuccess = trainsAtStationList.removeOne(trainID);
  if(removalSuccess)
  {
    emit qmlStationOccupancySignal(this->getName(), trainsAtStationList.length(), numberOfPlatforms);
  }
  else
  {
    qDebug()<<"ERROR  : Train departure failed";
  }
  return removalSuccess;
}

Station::~Station()
{
}
