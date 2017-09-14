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
  maxAllowedSpeed = 60;
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
 * Adds train tothe train network. The location of the train will be set in
 *
 * @param sTrack The Track object that acts as the original for this object.
 */
bool Track::addTrain(int trainID){
  trainsOnTrackQueue.enqueue(trainID);
  emit qmlTrackStatusSignal(this->getName(), trainsOnTrackQueue.length(), "AVAILABLE");
  return false;
}

bool Track::deleteTrain(int trainID){
  if(trainsOnTrackQueue.isEmpty()){
    qDebug()<<"ERROR  : Trying to remove rom an empty queue";
    return false;
  } else {
    if(trainsOnTrackQueue.head() == trainID){
      trainsOnTrackQueue.dequeue();
      emit qmlTrackStatusSignal(this->getName(), trainsOnTrackQueue.length(), "AVAILABLE");
      return true;
    } else {
      qDebug()<<"CRASH!!! : Queue violation!";
    }
  }
  qDebug()<<"INFO   : DELETE LAST TRAIN "<<trainID<<" FROM TRACK: "<<trackID;
  return false;
}

//TODO: CHECK FOR COORDINATES!
QList<float> Track::getCoordinatesFromPosition(int position)
{
  int iii;
  float fractionOfLeg;
  QList<float> thisList;

  for (iii=0; iii < coordinateList.length(); iii++){
    if (coordinateCumulatedDistanceList.at(iii) >= position){
      iii--;
      break;
    }
  }
  fractionOfLeg = ( (float) position-coordinateCumulatedDistanceList.at(iii))/
                  (coordinateCumulatedDistanceList.at(iii+1)-coordinateCumulatedDistanceList.at(iii));
  thisList<< (coordinateList.at(iii*2+2)-coordinateList.at(iii*2)) * fractionOfLeg + coordinateList.at(iii*2);
  thisList<< (coordinateList.at(iii*2+2+1)-coordinateList.at(iii*2+1)) * fractionOfLeg + coordinateList.at(iii*2+1);
  return thisList;
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
 * The method returns the maximum allowed speed for this Track object.
 * The unit is meters per second.
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
 * @return startStation The station ID for the startStation for this Track
 *                      object.
 */
int Track::getStartStation() { return startStation; }

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
  if (startStation != UNDEFINED)
  {
    message << "S[" + QString::number(startStation) + "]" +
            thisStationList->at(startStation)->getName();
  }
  else { message << "S[-1] N/A";}
  if (endStation != UNDEFINED) {
    message << "S[" + QString::number(endStation) + "] " +
            thisStationList->at(endStation)->getName(); }
  else { message<<"S[-1] N/A";}
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
 * The method sets the maximum allowed speed for the Track object.
 *
 * @todo Implement ability to have different allowed max speed for different
 *       parts of the Track object.
 * @todo Investigate how the track will handle negative numbers for max allowed speed.
 * @param n The maximum allowed speed [m/s].
 */
void Track::setMaxAllowedSpeed(int n) { maxAllowedSpeed = n; }

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
