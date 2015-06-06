#include <iostream>
#include<sstream>
#include<cstring>
#include<vector>
#include<QString>
#include<QDebug>
#include <QtCore>
#include "tctrain.h"
#include "trafficcontrol.h"
#define TRUE 1//Replace with constants that are global!
#define FALSE 0
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
Train::Train(QString tn, QList<Track*>& trackList, QList<Train*>& trainList, QList<Station*>& stationList)
{
    trainName=QString::fromUtf16(tn.utf16());
    nbrOfSeats=100;
	positionOnTrack=0;
  	nbrOfPassengers=0;
	desiredSpeed=0;
	currentSpeed=0;
	currentStation=-1;
	currentTrack=-1;
	nextTrack=-1;

    stateTable.insert("WAITING", 0); //More stations to visit
    stateTable.insert("READY", 1);   //Train is ready to start immediately
    stateTable.insert("RUNNING", 2); //Train is running
    stateTable.insert("OPENING", 3); //Doors are opening
    stateTable.insert("LOADING", 4); //Doors are open, loading passengers
    stateTable.insert("CLOSING", 5); //Doors are closing
    stateTable.insert("STOPPED", 6); //No more stations to visit or emergency brake

	trainID=totalNbrOfTrains;/*!<The trainID will be correspond to the order of created Train objects. */
    totalNbrOfTrains++;
    nextIndexTravelPlanByStationID=0;
	maxSpeed=40;
    thisTrackList=&trackList;
    thisTrainList=&trainList;
    thisStationList=&stationList;
    passing=false;
}

/*!
 * Adds a Track ID to the trainRoute in order to define the route for the train.
 *
 * @todo Integrity check to make sure that the track is valid.
 * @todo investigate if it is better to use a list of stations instead.
 * 
 * @param trackID the ID number of the track to add.
 */
void Train::addStationToTrainRoute(int trackID)
{
//	For trafficcontrol:Check if track exists and can be reached from the prior track.
    if (trackID<=0){
        qDebug()<<"Train::addTrackToTrainRoute Error: negative number specified.\n";
    } else {
        qDebug()<<"Adding track to trainroute";
        travelPlanByStationID.push_back(trackID);
	}
	sendDataChangedSignal(trainID);
}

/*!
 * Adds a Track ID to the trainRoute in order to define the route for the train.
 *
 * @todo Integrity check to make sure that the track is valid.
 * @todo investigate if it is better to use a list of stations instead.
 * 
 * @param trackID the ID number of the track to add.
 */
int addStationToTrainRoute(QString stationName, bool stopAtStation)
{
    qDebug()<<"Add station"<< stationName<< " to train route. Stop="<<stopAtStation ;
    return 1;
}

/*!
 * Get the ID of the current Track object for the Train object. 
 *
 * @return currentTrack. The value -1 indicates that the Train is not at a Track (most likely on a Station).
 */
int Train::getCurrentTrack() { return currentTrack; }

/*!
 * Get the ID of the current Station object for the Train object.
 *
 * @return currentStation. The value -1 indicates that the Train is not at a Station (most likely on a Track).
 */
int Train::getCurrentStation() { return currentStation; }

/*!
 * Get the actual speed of the Train object.
 *
 * @return currentSpeed the current speed [m/s].
 */
int Train::getCurrentSpeed() { return currentSpeed; }

/*!
 * Get the desired/ordered speed of the Train object.
 *
 * @return desiredSpeed the desired speed [m/s].
 */
int Train::getDesiredSpeed() { return desiredSpeed; }

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
vector<int> Train::getRouteVectorStations() {return travelPlanByStationID;}

/*!
 * Get the current location on the Train's trip plan. The method getTrainRouteVector will give the entire trip plan.
 *
 * @return trainRouteIndex The position of the Train's trip plan.
 */
int Train::getIndexTravelPlanByStationID() {return nextIndexTravelPlanByStationID;}

/*!
 * Move the Train. The method will iterate until the timer is zero or the Train is blocked. In that case, it will return the remaining time to move.
 *
 * @param n Time in seconds that the Train shall move
 * 
 * @return n Remaining time to move later. If the method concludes that the Train is blocked, it will return the remaining time to move. 0 indicates that the move operation is complete.
 */
int Train::move(int n)
//This function must be able to move to, from and through one or several stations.
{
    QMutexLocker locker(&mutex);
    while (n>0){
        if(state==stateTable.value("WAITING"))
        {
            state=stateTable.value("READY");
        } else if(state==stateTable.value("READY"))
        {
            if(currentStation!=-1) {//Train at station.
                if(nextTrack==-1){//Next track not defined
                    nextTrack=thisStationList->at(currentStation)
                        //THIS SHOULD BE REPLACED BY A FUNCTION OR METHOD THAT SEARCHES FOR TRACK
                        ->findLeavingTrackIndexToStation(//What track to next station?
                            travelPlanByStationID.at(nextIndexTravelPlanByStationID));//Next station in travelplan
                    if(nextTrack==-1){//Still no suitable track found - IS THIS STATE POSSIBLE?
                        if(travelPlanByStationID.at(nextIndexTravelPlanByStationID)==currentStation){
                            state=stateTable.value("STOPPED");
                            return 0;
                        } else {
                            qDebug()<<"Tr:M:ERROR! No track from "<<thisStationList->at(currentStation)->getName()
                               <<" to "<<thisStationList->at(travelPlanByStationID
                                                            .at(nextIndexTravelPlanByStationID))
                                                            ->getName();
                            this->showInfo();
                            state=stateTable.value("STOPPED");
                            return 0;
                        }
                    }
                }
                if(thisStationList->at(currentStation)->checkIfTrackLeavesStation(nextTrack)==TRUE){//Train is at a station and next track is leaving the station.
                    currentTrack=nextTrack;
                    currentStation=-1;//Remove this train from station.
                    nextTrack=-1;
                    positionOnTrack=0;//CAUTION!!!
                    state=stateTable.value("RUNNING");
                    if(nextIndexTravelPlanByStationID < ( ( (int) travelPlanByStationID.size() ) - 1 ) ) { nextIndexTravelPlanByStationID++; }//target next station
                } else {//Try to find leaving track to nextStation
                    nextTrack=thisStationList->at(currentStation)
                            ->findLeavingTrackIndexToStation(travelPlanByStationID.at(nextIndexTravelPlanByStationID));
                    if(nextTrack==-1){
                        qDebug()<<"TR:M(172): ERROR! No track from "<<thisStationList->at(currentStation)->getName()
                               <<" to "<<thisStationList->at(travelPlanByStationID
                                                            .at(nextIndexTravelPlanByStationID))
                                                            ->getName();
                        this->showInfo();
                        state=stateTable.value("STOPPED");
                    }
                    sendDataChangedSignal(trainID);
                    return n;
                }
            }
        } else if(state==stateTable.value("RUNNING"))
        {
            if(positionOnTrack==thisTrackList->at(currentTrack)->getLength())	{ //Enter station
                currentStation=thisTrackList->at(currentTrack)->getEndStation();
                currentSpeed=0;
                currentTrack=-1;
                state=stateTable.value("OPENING");
                doorOpenProgress=0;
            } else {
                //qDebug()<<"Tr:M: On a track, still some distance to the end of track.";
                desiredSpeed=(int)min((double)maxSpeed, sqrt((thisTrackList->at(currentTrack)->getLength()-positionOnTrack-currentSpeed)*2));
                desiredSpeed=max(desiredSpeed,1);
                if(currentSpeed!=desiredSpeed)  { currentSpeed=currentSpeed+(desiredSpeed-currentSpeed)/(abs(desiredSpeed-currentSpeed)); }
                this->setTrackPosition(positionOnTrack+currentSpeed);
            }
            n--;
        } else if(state==stateTable.value("OPENING"))
        {
            doorOpenProgress+=20;
            if(doorOpenProgress==100){
                state=stateTable.value("LOADING");
            }
            n--;
        } else if(state==stateTable.value("LOADING"))
        {
            //Train shall continue loading passengers until no more passengers are on station
            if(thisStationList->at(currentStation)->getNbrOfWaitingPassengers()==0)
            {
                state=stateTable.value("CLOSING");
                doorOpenProgress=100;
                n--;
            }
        } else if(state==stateTable.value("CLOSING"))
        {
            doorOpenProgress-=20;
            if(doorOpenProgress==0){
                state=stateTable.value("WAITING");
            }
            n--;

        } else if(state==stateTable.value("STOPPED"))
        {
        }
        n--;//REMOVE!!!
    }
    sendDataChangedSignal(trainID);//INVESTIGATE WHEN DATACHANGEDSIGNAL SHALL BE SENT!
    return n;
}

/*!
 * Set the current station for the Train. 
 *
 * @param stationID The ID for the station that will be the current station for this Train.
 */
void Train::setCurrentStation(int stationID) { 
	currentStation=stationID; 
    state=stateTable.value("LOADING");

    sendDataChangedSignal(trainID);//+1?
}//Later: reserve place in station for the train

/*!
 * Set the desired speed for the Train. 
 *
 * @param n The desired speed for the Train [m/s].
 */
void Train::setDesiredSpeed(int n) { 
	desiredSpeed=min(maxSpeed, n); 
	if(currentTrack!=-1){
        int maxSpeed=thisTrackList->at(currentTrack)->getMaxAllowedSpeed();
		desiredSpeed=std::min(n, maxSpeed );
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
 * Set the position on the current Track for the train in meters. This is done only if the train is located on a track. After completion, a signal is sent.
 *
 * @param n The desired position for the train [m].
 */
void Train::setTrackPosition(int n)
{
	if (currentTrack!=-1){
		positionOnTrack=max(n, 0);
        positionOnTrack=min(positionOnTrack, thisTrackList->at(currentTrack)->getLength());
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
 * The method prepares a message and sends it to the Train data model. The signal contains the name of the train, the current station/track, the position on the track and the current speed.
 *
 * @param trainID The ID of the Train object that has information to update.
 */
void Train::sendDataChangedSignal(int trainID){//Name, Track/Station , Position, Speed
	QStringList message;
    message<<trainName;
    if ( ( ( currentStation!=-1 ) && (currentTrack!=-1) ) || ( (currentTrack==-1) && (currentStation==-1) ) ){ message<<"ERROR: Invalid position!";}
	if( currentStation!=-1 ) { 
        message<<"S["+QString::number(currentStation)+"] "+ thisStationList->at(currentStation)->getName(); }
	else if (currentTrack!=-1){
        message<<"T["+QString::number(currentTrack)+"] "+thisTrackList->at(currentTrack)->getName();	}
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
 	currentTrack=trackID; 
    sendDataChangedSignal(trainID); //+1?
}

/*!
 * The method sets the current name of the Train object.
 *
 * @param name The new name of the Train object that has information to update.
 */
void Train::setName(QString const& tn) {
    trainName=QString::fromUtf16(tn.utf16());
    sendDataChangedSignal(trainID); //+1?
}

/*!
 * The method shows the information of the train to the console.
 */
void Train::showInfo()
{
	int iii=0;
 	vector<int>::iterator it;
    qDebug()<<"Car has "<<nbrOfSeats<<" seats.  " << nbrOfPassengers <<" are taken. ";
	if(currentTrack!=-1)	{	
        qDebug()<<"CurrentTrack is not null";
        qDebug()<<"Speed: "<<currentSpeed<<" m/s, track:"<<thisTrackList->at(currentTrack)->getName()<< " at position "<<positionOnTrack<<". ";
	}
    if(currentStation!=-1)
    {
        qDebug()<<"Train is at station "<<thisStationList->at(currentStation)->getName();
    }
    qDebug()<<"TrainRoute, index: "<<nextIndexTravelPlanByStationID<<" : ";
    for ( it = travelPlanByStationID.begin(); it != travelPlanByStationID.end(); ++it ) {
		qDebug()<<" "<<*it;
        if(nextIndexTravelPlanByStationID==iii){cout<<"*";}
		iii++;
	}
}

Train::~Train()
{
}
