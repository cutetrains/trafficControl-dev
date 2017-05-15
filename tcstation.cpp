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
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <qDebug>
#include "tcstation.h"
#include "trafficcontrol.h"
#define UNDEFINED -1

using namespace std;
int Station::totalNbrOfStations = 0;

/*!
 * The constructor for a Station object.
 *
 * @param cn Name of the station
 * @TODO Rename cn to a more readable name
 * @param trackList is a pointer to the trackList
 * @param trainList is a pointer to the trackList
 * @param stationList is a pointer to the stationList
 */
Station::Station(QString cn,
                 bool isJunction,
                 QList<Track*>& trackList,
                 QList<Train*>& trainList,
                 QList<Station*>& stationList)
{ 	
  name = QString::fromUtf16(cn.utf16());
  waitingPassengers = 0;
  /*The stationID will be correspond to the order of created Station objects.*/
  stationID = totalNbrOfStations;
  totalNbrOfStations++;
  thisTrackList = &trackList;
  thisTrainList = &trainList;
  thisStationList = &stationList;
}	

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
void Station::addTrack(int trackID) { 
  if (trackID<= -1 ){
    qDebug()<<"ERROR  : Station::addTrack" << name
            <<" Error: negative number specified: "<<trackID;
  }
  else
  {
    leavingTrackVector.push_back(trackID);
  }
  sendDataChangedSignal(stationID);
}

/*!
 * The method checks if track trackID is listed in the leavingTrackVector.
 *
 * @param trackID The ID of the track that shall be added to the list of
 *                leaving tracks for the station.
 * @todo: Check if foreach can simplify the code
 * 
 * @return bool true if the track is listed as leaving for this station, false
 *              otherwise.
 */
bool Station::checkIfTrackLeavesStation(int trackID)
{
  bool foundTrack = false;
  vector<int>::iterator it;
  for (it =leavingTrackVector.begin(); it != leavingTrackVector.end(); ++it) {
    if(trackID == *it)
    {
      foundTrack = true;
    }
  }
  return foundTrack;
}

/*!
 * The method finds the track index in leavingTrackVector to the adjacent
 * station stationID. If no tracks are found, or the station isn't adjacent,
 * the method will return -1.
 *
 * @param stationID The ID of the station to rearch.
 *
 * @return trackID The ID number the station.
 * @TODO: Search for tracks in opposite direction.
 */
int Station::findLeavingTrackIndexToStation(int targetStationID)
{
  foreach(int i, leavingTrackVector)
  {
    if((thisTrackList->at(i)->getEndStation()) == targetStationID)
    {
      return i;
    }
  }
  return UNDEFINED;
}

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
 * The method shows information about the Station object (Name, number of
 * waiting passengers, number of leaving tracks) to the console.
 * @todo Check if a foreach statement can simplify the code.
 */
void Station::showInfo() {
  qDebug() << name << " (" << stationID << ") has " << waitingPassengers
           << " waiting passengers. ";
  vector<int>::iterator it;
  qDebug() << "INFO   : Leaving tracks: ";
  for (it = leavingTrackVector.begin(); it != leavingTrackVector.end(); ++it) {
    qDebug() <<"        "<<*it;
  }
  qDebug() << "INFO : thisTrackList: Size is " << thisTrackList->size()
           << "thisStationList: Size is " << thisStationList->size()
           << " thisTrainList: Size is " << thisTrainList->size() << "\n";
}

/*!
 * The adds a number of passengers to the current number of waiting passengers
 * and notifies the model.
 *
 * @param nbrPassengersToAdd The number of passengers that shall be added to
 * the station. A negative number means that some passengers are leaving the station.
 */
void Station::changeNbrOfPassengers(int nbrPassengersToAdd) { 
  waitingPassengers = max((waitingPassengers + nbrPassengersToAdd), 0);
  sendDataChangedSignal(stationID); //+1?
}

Station::~Station()
{
}
