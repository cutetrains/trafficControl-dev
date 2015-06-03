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
#include<sstream>
#include<cstring>
#include<vector>
#include<qDebug>
#include "tcstation.h"
#include "trafficcontrol.h"
#define TRUE 1//Replace with constants that are global!
#define FALSE 0

using namespace std;
int Station::totalNbrOfStations =0; 

/*!
 * The constructor for a Station object.
 *
 * @param cn Name of the station
 * @param trackList is a pointer to the trackList
 * @param trainList is a pointer to the trackList
 * @param stationList is a pointer to the stationList
 */
Station::Station(QString cn, QList<Track*>& trackList, QList<Train*>& trainList, QList<Station*>& stationList) 
{ 	
	name=QString::fromUtf16(cn.utf16());

  	waitingPassengers=0;

	stationID=totalNbrOfStations;/*!<The stationID will be correspond to the order of created Station objects. */
    totalNbrOfStations++;
    thisTrackList=&trackList;
    thisTrainList=&trainList;
    thisStationList=&stationList;
}	

/*!
 * The method returns the number of waiting passengers at the current station
 * @todo When passengers have different destinations, a more advanced method is needed. That needs to list passengers according to their desired destinations.
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
 * The method adds a track, trackID, to the vector of leaving tracks for the station. Currently, only leaving tracks are recorded in the object, not incoming tracks.
 *
 * @param trackID The ID of the track that shall be added to the list of leaving tracks for the station.
 * @todo Check if track exists already and check if track is already leaving this or other stations, using checkIfTrackLeavesStation for all stations
 * @todo Check if track is already connected to the station
 */
void Station::addTrack(int trackID) { 
    if (trackID<=-1){
        qDebug()<<"Station::addTrack" <<name<<" Error: negative number specified: "<<trackID;
	} else {
		leavingTrackVector.push_back(trackID);
	}
    sendDataChangedSignal(stationID);
}

/*!
 * The method checks if track trackID is listed in the leavingTrackVector.
 *
 * @param trackID The ID of the track that shall be added to the list of leaving tracks for the station.
 * @todo: Check if foreach can simplify the code
 * 
 * @return bool TRUE if the track is listed as leaving for this station, FALSE otherwise.
 */
bool Station::checkIfTrackLeavesStation(int trackID)
{
	bool foundTrack=FALSE;
	vector<int>::iterator it;
	for ( it =leavingTrackVector.begin(); it != leavingTrackVector.end(); ++it ) {
		if(trackID==*it) {foundTrack=TRUE;} 		
	}
	return foundTrack;
}

/*!
 * The method finds the track index in leavingTrackVector to the adjacent station stationID. If no tracks are found, or the
 * station isn't adjacent, the method will return -1.
 *
 * @param stationID The ID of the station to rearch.
 *
 * @return trackID The ID number the station.
 */
int Station::findLeavingTrackIndexToStation(int targetStationID)
{
    int tempStationID=-1;
    QString tmpStr;
    foreach(int i, leavingTrackVector)
    {
        tempStationID=(thisStationList->at(thisTrackList->at(i)->getEndStation())->getID());
        if((thisTrackList->at(i)->getEndStation()) == targetStationID){
            return i;
        }else{}
//      "Fortsätt här!";
        //if (thisTrackList.at(thisTrackID)->getEndStation()==endStationID);
    }
    return -1;
}

/*!
 * The method prepares a message and sends it to the Station data model. The signal contains the name of the station and the number of waiting passengers.
 * @todo Check how Doxygen handles signals/slots in the documentation.
 *
 * @param stationID The ID of this Station object.
 */
void Station::sendDataChangedSignal(int stationID){//Name, Track/Station , Position, Speed
	QStringList message;
	QString tmp;
	message<<name; 
	message<<QString::number(waitingPassengers);
    emit dataChangedSignal(stationID, QVariant(message));
}

/*!
 * The method shows information about the Station object (Name, number of waiting passengers, number of leaving tracks) to the console.
 * @todo Check if a foreach statement can simplify the code.
 */
void Station::showInfo() { 
    qDebug() << name <<" ("<<stationID<<") has "<<waitingPassengers<<" waiting passengers. ";
 	vector<int>::iterator it;
	cout<<"Leaving tracks: ";
	for ( it = leavingTrackVector.begin(); it != leavingTrackVector.end(); ++it ) {
		cout <<", "<<*it; 
	}
    qDebug()<<"thisTrackList: Size is "<<thisTrackList->size()<<"thisStationList: Size is "<<thisStationList->size()<<" thisTrainList: Size is "<<thisTrainList->size();
	cout <<"\n";
}

/*!
 * The adds a number of passengers to the current number of waiting passengers and notifies the model. 
 *
 * @param nbrPassengersToAdd The number of passengers that shall be added to the station. A negative number means that some passengers are leaving the station.
 */
void Station::changeNbrOfPassengers(int nbrPassengersToAdd) { 
	waitingPassengers = max((waitingPassengers + nbrPassengersToAdd), 0);
    sendDataChangedSignal(stationID); //+1?
}

Station::~Station()
{
}
