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
#include<sstream>
#include<cstring>
#include<vector>
#include<QDebug>
#include "tctrack.h"
#include "trafficcontrol.h"
using namespace std;
int Track::totalNbrOfTracks=0; 

/*!
 * The constructor for a Track object.
 *
 * @param cn Name of the track
 * @param nn Length of the track [m]
 * @param trackList is a pointer to the trackList
 * @param trainList is a pointer to the trackList
 * @param stationList is a pointer to the stationList
 */
Track::Track(QString cn, int nn,QList<Track*>& trackList, QList<Train*>& trainList, QList<Station*>& stationList)
{
  	length=nn;
	name=QString::fromUtf16(cn.utf16());
  	startStation=-1;
	endStation=-1;
    maxAllowedSpeed=60;
	trackID=totalNbrOfTracks;/*!<The trackID will correspond to the order of created Track objects. */
    totalNbrOfTracks++;
    thisTrackList=&trackList;
    thisTrainList=&trainList;
    thisStationList=&stationList;
}

/*!
 * The assign constructor for the Track object.
 *
 * @param sTrack The Track object that acts as the original for this object.
 */
Track::Track( const Track& sTrack)
	:QObject()
{
	length=sTrack.length;
    name=QString::fromUtf16(sTrack.name.utf16());
	startStation=sTrack.startStation;
	endStation=sTrack.endStation;
	maxAllowedSpeed=sTrack.maxAllowedSpeed;
	trackID=sTrack.trackID;
    thisTrackList=sTrack.thisTrackList;
    //qDebug()<<"WARNING: copy constructor assigns by value!";
    thisTrainList=sTrack.thisTrainList;
    thisStationList=(sTrack.thisStationList);
}

/*!
 * The copy constructor for the Track object.
 *
 * @param sTrack The Track object that acts as the original for this object.
 */
Track& Track::operator=( const Track& sTrack )  
{
	length=sTrack.length;
	name=QString::fromUtf16(sTrack.name.utf16());
	startStation=sTrack.startStation;
	endStation=sTrack.endStation;
	maxAllowedSpeed=sTrack.maxAllowedSpeed;
	trackID=sTrack.trackID;
	thisTrackList=sTrack.thisTrackList;
	thisTrainList=sTrack.thisTrainList;
	thisStationList=sTrack.thisStationList;	
	return *this;
}

/*!
 * The method returns the end station for this Track object.
 *
 * @return endStation The station ID for the endStation for this Track object.
 */
int Track::getEndStation() { return endStation; }

/*!
 * The method returns the ID number for this Track object.
 *
 * @return trackID The ID number of the Track object.
 */
int Track::getID() { return trackID;}


/*!
 * The method returns the length for this Track object. The unit is meters.
 *
 * @return length The length for this Track object.
 */
int Track::getLength() { return length;}

/*!
 * The method returns the maximum allowed speed for this Track object. The unit is meters per second.
 *
 * @return length The length in meters for this Track object.
 */
int Track::getMaxAllowedSpeed() { return maxAllowedSpeed ; }

/*!
 * The method returns name for this Track object.
 *
 * @return name The name for this Track object.
 */
QString Track::getName() {return name;}

/*!
 * The method returns the start station for this Track object.
 *
 * @return startStation The station ID for the startStation for this Track object.
 */
int Track::getStartStation() { return startStation; }

/*!
 * The method prepares and sends a dataChangedSignal to trafficDataModel to update it.
 *
 * @param trackID The ID of the Track object that has changed.
 */
void Track::sendDataChangedSignal(int trackID){
	QStringList message;
	message<<name; //Track Name
	message<<QString::number(length); //Track Length
	if( startStation!=-1) { 
        message<<"S["+QString::number(startStation)+"] "+ thisStationList->at(startStation)->getName(); }
	else { message<<"S[-1] N/A";}
	if( endStation!=-1) { 
        message<<"S["+QString::number(endStation)+"] "+ thisStationList->at(endStation)->getName(); }
	else { message<<"S[-1] N/A";}
    emit dataChangedSignal(trackID, QVariant(message));

}

/*!
 * The method sets the startStation of the Track object.
 *
 * @param stationID The ID of the Station object that will be the start station.
 */
void Track::setStartStation(int stationID) { 
    startStation=stationID;
    sendDataChangedSignal(trackID);
}

/*!
 * The method sets the endStation of the Track object.
 *
 * @param stationID The ID of the Station object that will be the end station.
 */
void Track::setEndStation(int stationID) {	
    endStation=stationID;
    sendDataChangedSignal(trackID);
}

/*!
 * The method sets the maximum allowed speed for the Track object.
 *
 * @todo Implement ability to have different allowed max speed for different parts of the Track object.
 * @todo Investigate how the track will handle negative numbers for max allowed speed.
 * @param n The maximum allowed speed [m/s].
 */
void Track::setMaxAllowedSpeed(int n) { maxAllowedSpeed = n; }

/*!
 * The method prints information about the track to the console.
 */
void Track::showInfo()
{
    qDebug()<<"INFO   : TrackID: "<<name<<", Length:  "<<length<<", Max Speed: "<<maxAllowedSpeed<<".";
	if (startStation!=-1)
	{
        qDebug()<<"INFO   : StartStation ["<<startStation<<"] ";
	} else {	
        qDebug()<<"ERROR  : No start station is defined. ";
	}
	if (endStation!=-1)
	{
        qDebug()<<"INFO   : EndStation ["<<endStation<<"] ";
	} else {	
        qDebug()<<"WARNING: No end station is defined. ";
	}
    qDebug()<<"thisTrackList: Size is "<<thisTrackList->size()<<"thisStationList: Size is "<<thisStationList->size()<<" thisTrainList: Size is "<<thisTrainList->size();
}

Track::~Track()
{
}
