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

/*! @file tctrack.cpp
 * @class Track
 *
 * @brief Implementation of the Track class
 *
 * The class contains the implementation of the Track object
 *
 * @author Gustaf Brännberg
 *
 * Contact: gustaf_brannberg@hotmail.com
 */

#include <QDebug>
#include <QtMath>
#include "../inc/tctrack.h"
#include "../inc/trafficcontrol.h"
#define UNDEFINED -1
using namespace std;
int Track::totalNbrOfTracks=0; 

/*!
 * The constructor for a Track object.
 *
 * @param trackName Name of the track
 * @param trackLength Length of the track [m]
 * @param coordinates List of coordinates for the track
 * @param trackList is a pointer to the trackList
 * @param trainList is a pointer to the trackList
 * @param stationList is a pointer to the stationList
 */
Track::Track(QString trackName,
             int trackLength,
             QStringList coordinates,
             QList<Track*>& trackList,
             QList<Train*>& trainList,
             QList<Station*>& stationList)
{
  length = trackLength;
  name = trackName;
  startStation = UNDEFINED;
  endStation = UNDEFINED;
  lockedDownStream = false;
  lockedUpStream = false;
  maxAllowedSpeed = 60;
  reversedTraffic = false;
  trackID = totalNbrOfTracks;/*!<The trackID will correspond to the order of created Track objects. */
  totalNbrOfTracks++;
  thisTrackList = &trackList;
  thisTrainList = &trainList;
  thisStationList = &stationList;
  if ( !coordinates.isEmpty() )
  {
    float R = float(6372.8);
    float dLat;
    float dLon;
    float lat1;
    float lat2;
    float dist=0;
    for(int iii = 0; iii < (coordinates.length()/2); iii++ )
    {
      coordinateList<<coordinates.at(iii*2).toDouble();
      coordinateList<<coordinates.at(iii*2+1).toDouble();
      if(iii>0)
      {
        //Use Haversine to calculate delta
        dLat = (coordinateList.at(iii*2)-coordinateList.at(iii*2-2))*3.1415926536 / 180;
        dLon = (coordinateList.at(iii*2+1)-coordinateList.at(iii*2-2+1))*3.1415926536 / 180;
        lat1 = coordinateList.at(iii*2-2)*3.1415926536 / 180;
        lat2 = coordinateList.at(iii*2)*3.1415926536 / 180;
        dist+=R*2*asin(sqrt(sin(dLat/2)*sin(dLat/2)+cos(lat1)*cos(lat2)*sin(dLon/2)*sin(dLon/2)));
      }
      coordinateCumulatedDistanceList<<floor(dist*1000);
    }
    hasValidCoordinates = true;
  }
  else
  {
    hasValidCoordinates = false;
  }
}

/*!
 * The assign constructor for the Track object.
 * @TODO: Add coordinatelist
 *
 * @param sTrack The Track object that acts as the original for this object.
 */
Track::Track( const Track& sTrack)
  :QObject()
{
  length = sTrack.length;
  name = sTrack.name;
  startStation = sTrack.startStation;
  endStation = sTrack.endStation;
  lockedDownStream = sTrack.lockedDownStream;
  lockedUpStream = sTrack.lockedUpStream;
  hasValidCoordinates = sTrack.hasValidCoordinates;
  maxAllowedSpeed = sTrack.maxAllowedSpeed;
  trackID = sTrack.trackID;
  coordinateList = sTrack.coordinateList;
  coordinateCumulatedDistanceList = coordinateCumulatedDistanceList;
  thisTrackList = sTrack.thisTrackList;
  thisTrainList = sTrack.thisTrainList;
  thisStationList = sTrack.thisStationList;
}

/*!
 * The copy constructor for the Track object.
 *
 * @param sTrack The Track object that acts as the original for this object.
 */
Track& Track::operator=( const Track& sTrack )  
{
  length = sTrack.length;
  name = sTrack.name;
  startStation = sTrack.startStation;
  endStation = sTrack.endStation;
  lockedDownStream = sTrack.lockedDownStream;
  lockedUpStream = sTrack.lockedUpStream;
  hasValidCoordinates = sTrack.hasValidCoordinates;
  maxAllowedSpeed = sTrack.maxAllowedSpeed;
  trackID = sTrack.trackID;
  coordinateList = sTrack.coordinateList;
  coordinateCumulatedDistanceList = coordinateCumulatedDistanceList;
  thisTrackList = sTrack.thisTrackList;
  thisTrainList = sTrack.thisTrainList;
  thisStationList = sTrack.thisStationList;
  return *this;
}

/*!
 * Adds train to the track.
 *
 * @param trainID The id of the train to add.
 *
 * @return bool telling whether it was possible to add the train.
 *
 * @todo Check for errors
 */
bool Track::addTrainToTrack(int trainID){
  reversedTraffic = (endStation == thisTrainList->at(trainID)->getCurrentStation() );
  trainsOnTrackQueue.append(trainID);
  if(reversedTraffic)
  {
    thisTrainList->at(trainID)->setTrackPosition(length);
    setLockDownStream(true);
  }
  else
  {
    thisTrainList->at(trainID)->setTrackPosition(0);
    setLockUpStream(true);
  }

  return true;
}

/*!
 * Delete train from the list of trains on the track.
 *
 * @param trainID The id of the train to remove
 *
 * @return bool telling whether it was possible to add the train.
 *
 * @todo Add Assert
 * @todo Analyse whether this is called or not
 */
bool Track::deleteTrainFromTrack(int trainID){
  if(trainsOnTrackQueue.isEmpty()){
    return false;
  }
  else
  {
    if(trainsOnTrackQueue.first() == trainID){
      trainsOnTrackQueue.removeFirst();
      if(trainsOnTrackQueue.isEmpty())
      {
        reversedTraffic = false;
        setLockDownStream(false);
        setLockUpStream(false);
        emit qmlTrackStatusSignal(this->getName(), trainsOnTrackQueue.length(), "FREE");
      }

      return true;
    }
    else
    {
      qDebug()<<"CRASH!!! : Queue violation!";
    }
  }
  return false;
}

/*!
 * The method is only used when deleting the networkControl object and will
 * set the total number ofstations to zero. This is done mainly in the tests
 * to ensure that the test environment is reset.
 */
void Track::destructorResetTotalNumberOfTracks() { totalNbrOfTracks = 0; }

/*!
 * The method calculates the coordinates for a position on the track.
 *
 * @param position Position along the track in meters.
 *
 * @return The coordinates for the position.
 */
QList<float> Track::getCoordinatesFromPosition(int position)
{
  int iii;
  float fractionOfLeg;
  QList<float> returnList;
  if(!this->hasValidCoordinates)
  {
    qDebug()<<"ERROR   : Trying to get coordinates from "<< this->getName();
    qDebug()<<"that has no coordinates";
    returnList<<0.0<<0.0;
    return returnList;
  }
  if(0 == position){
    returnList<<coordinateList.at(0)<<coordinateList.at(1);
    return returnList;
  }
  for (iii=0; iii < coordinateList.length(); iii++){
    if (coordinateCumulatedDistanceList.at(iii) >= position){
      iii--;
      break;
    }
  }
  fractionOfLeg = ( (float) position-coordinateCumulatedDistanceList.at(iii))/
                  (coordinateCumulatedDistanceList.at(iii+1)-coordinateCumulatedDistanceList.at(iii));
  returnList << (coordinateList.at(iii*2+2)-coordinateList.at(iii*2)) * fractionOfLeg + coordinateList.at(iii*2);
  returnList << (coordinateList.at(iii*2+2+1)-coordinateList.at(iii*2+1)) * fractionOfLeg + coordinateList.at(iii*2+1);
  return returnList;
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
 * The method returns the length for this Track object. [m]
 *
 * @return length The length for this Track object.
 */
int Track::getLength() { return length;}

/*!
 * The method returns the maximum allowed speed for this Track object. [m/s]
 *
 * @return maxAllowedSpeed The maximum allowed speed.
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
int Track::getStartStation(){ return startStation;}

/*!
 * The method returns the total number of tracks for this station.
 *
 * @return totalNbrOfTracks The number of tracks for this station.
 */
int Track::getTotalNbrOfTracks(){ return totalNbrOfTracks;}

/*!
 * The method returns a list of all trains on the track. The list is a FIFO queue.
 *
 * @return trainsOnTrackQueue A list of trains on the track.
 */
QList<int> Track::getTrainList()
{
  return trainsOnTrackQueue;
}

/*!
 * The method tells whether the track has valid coordinates or not.
 *
 * @return hasValidCoordinates TRUE if there are valid coordinates.
 */
bool Track::hasCoordinates(){ return hasValidCoordinates;}

/*!
 * The method tells whether the track is locked in downStream (start station to end station).
 *
 * @return lockedDownStream TRUE if the track is locked downStream.
 */
bool Track::isLockedDownStream(){ return lockedDownStream; }

/*!
 * The method tells whether the track is locked in upStream (end station to start station).
 *
 * @return lockedUpStream TRUE if the track is locked upStream.
 */
bool Track::isLockedUpStream(){ return lockedUpStream; }

/*!
 * The method returns whether the track operates in reversed directon (from end to start).
 *
 * return reversedTraffic TRUE if the traffic is in reversed direction.
 */
bool Track::isReversedTraffic() { return reversedTraffic; }

/*!
 * The method prepares and sends a dataChangedSignal to trafficDataModel to
 * update it.
 *
 * @param trackID The ID of the Track object that has changed.
 */
void Track::sendDataChangedSignal(int trackID){
  QStringList message;
  message << name;
  message << QString::number(length);
  //TODO: Check that stationID is within upper range too
  if(isLockedDownStream()){
    if(isLockedUpStream()){
      message << "Locked";
    } else {
      message << "Start";
    }
  } else {
    if(isLockedUpStream()){
      message << "End";
    } else {
      message << "Free";
    }
  }
  if (startStation != UNDEFINED)
  {
    message << thisStationList->at(startStation)->getName();
  }
  else { message << "N/A";}
  if (endStation != UNDEFINED) {
    message << thisStationList->at(endStation)->getName(); }
  else { message<<"N/A";}
  emit dataChangedSignal(trackID,
                         QVariant(message));
}

/*!
 * The method sets the startStation of the Track object.
 *
 * @param stationID The ID of the Station object that will be the start station.
 */
void Track::setStartStation(int stationID) { 
  startStation = stationID;
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
 * The method sets a lock in downStream (from start to end).
 *
 * @param lockDS TRUE if the track shall be locked in downStream.
 */
void Track::setLockDownStream(bool lockDS) {
  lockedDownStream = lockDS;
  sendDataChangedSignal(this->getID());
  if(lockDS == true){
    emit qmlTrackStatusSignal(this->getName(), trainsOnTrackQueue.length(), "LOCKED_START");
  }
}

/*!
 * The method sets a lock in upStream
 *
 * @param lockUS TRUE if the track shall be locked in upStream.
 */
void Track::setLockUpStream(bool lockUS) {
  lockedUpStream = lockUS;
  sendDataChangedSignal(this->getID());
  if(lockUS == true){
    emit qmlTrackStatusSignal(this->getName(), trainsOnTrackQueue.length(), "LOCKED_END");
  }
}

/*!
 * The method sets the maximum allowed speed for the Track object.
 *
 * @todo Implement ability to have different allowed max speed for different
 *       parts of the Track object.
 * @todo Investigate how the track will handle negative numbers for max allowed speed.
 * @param n The maximum allowed speed [m/s].
 */
void Track::setMaxAllowedSpeed(int n) { maxAllowedSpeed = n; }

/*!
 * The method sets whether the track operates in reversed directon (from end to start).
 *
 * param rt true if the traffic is in reversed direction.
 */
void Track::setReversedTraffic(bool rt) { reversedTraffic = rt; }

/*!
 * The method prints information about the track to the console.
 */
void Track::showInfo()
{
  qDebug() << "INFO   : TrackID: " << name << ", Length:  " << length
           << ", Max Speed: " << maxAllowedSpeed << ".";
  if (startStation != UNDEFINED)
  {
    qDebug() << "INFO   : StartStation [" << startStation << "] ";
  } else {
    qDebug() << "ERROR  : No start station is defined. ";
  }
  if (endStation != UNDEFINED)
  {
    qDebug() << "INFO   : EndStation [" << endStation << "] ";
  } else {
    qDebug() << "WARNING: No end station is defined. ";
  }
    qDebug() << "thisTrackList: Size is " << thisTrackList->size()
             << "thisStationList: Size is " << thisStationList->size()
             << " thisTrainList: Size is " << thisTrainList->size();
}

Track::~Track()
{
}
