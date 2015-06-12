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
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include "TrafficControl.h"
#include "ui_TrafficControl.h"
#define TRACK 1//Replace with constants that are global!
#define TRAIN 2
#define STATION 3
#define TRUE 1//Replace with constants that are global!
#define FALSE 0
using namespace std;

/*!
 * This method imports a network from a file to the program.
 * @todo: If this part becomes too extensive, place this code in a separate source file.
 * @todo: Introduce consistensy checks for files.
 */
void TrafficControl::importPredefinedNetwork()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "C://temp//train//",
                                                     tr("Files (*.*)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Could not open file.";
    }

    int currentTrain=-1;
    int currentTrack=-1;

    QTextStream in(&file);
    while ( !in.atEnd() )
    {    
        QString line = in.readLine();
        QStringList argumentList=line.split(" ");
        bool approvedCommand=false;

        //Code for analysing track commands
        if ( ( argumentList.indexOf("TRACK")==0 ) && ( currentTrack == -1 ) ){
            qDebug()<<"FATAL ERROR: Train definition file modifies train before creating it!";
            exit(1);
        }
        if ( ( argumentList.indexOf("ADD") ==0 ) && ( argumentList.indexOf("TRACK") ==1 ) ) {
            if(argumentList.count()==4){
                bool ok=false;
                int iLength=argumentList.at(3).toInt(&ok,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
                if( ok==true){
                    addTrackToNetwork(argumentList.at(2), iLength);
                    approvedCommand=true;
                    currentTrack=trackList.length()-1;
        } } }
        if ( ( argumentList.indexOf("TRACK") ==0 ) && ( argumentList.indexOf("SELECT") ==1 ) ){
            if (argumentList.count()==3){
                foreach(Track* t, trackList){
                    if(t->getName()==argumentList.at(2)){
                        currentTrack=t->getID();
                        approvedCommand=TRUE;
        }   }   }   }
        if ( ( argumentList.indexOf("TRACK") ==0 ) && ( argumentList.indexOf("SET") ==1 ) && ( argumentList.indexOf("MAX_SPEED") ==2 )){
            if(argumentList.count()==4) {
                bool ok=false;
                int iSpeed=argumentList.at(3).toInt(&ok,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
                if( ok==true){
                    trackList.at(currentTrack)->setMaxAllowedSpeed(iSpeed);
                    approvedCommand=true;
        } } }
        if ( ( argumentList.indexOf("ADD") ==0 ) && ( argumentList.indexOf("STATION") ==1 ) ){
            if(argumentList.count()==3) {//Verify that the name is not a number.
                addStationToNetwork(argumentList.at(2));
                approvedCommand=true;
        } }
        if ( ( argumentList.indexOf("CONNECT") ==0 ) && ( argumentList.indexOf("TRACK") ==1 )&& ( argumentList.indexOf("FROM") ==3 )&& ( argumentList.indexOf("TO") ==5 )  && (argumentList.count()==7)){
            //Check that the second argument reflects an existing track, and that the stations exist.
            int foundTrack=-1;
            foreach(Track* t, trackList){
                if(t->getName()==argumentList.at(2)) {foundTrack=t->getID();}
            }
            int foundStartStation=-1;
            int foundEndStation=-1;
            foreach(Station* s, stationList){
                if(s->getName()==argumentList.at(4)){foundStartStation=s->getID();}
                if(s->getName()==argumentList.at(6)){foundEndStation=s->getID();}
            }
            if( ( foundStartStation!=-1)&& (foundEndStation != -1) && (foundStartStation!=foundEndStation) && (foundTrack !=-1)){
                connectTrackToStations(argumentList.at(2),argumentList.at(4),argumentList.at(6));
                approvedCommand=true;
            }
        }


        //Code for analysing train commands
        if ( ( argumentList.indexOf("TRAIN")==0 ) && ( currentTrain == -1 ) ){
            qDebug()<<"FATAL ERROR: Train definition file modifies train before creating it!";
            exit(1);
        }

        if ( ( argumentList.indexOf("ADD") ==0 ) && ( argumentList.indexOf("TRAIN") ==1 ) && ( argumentList.indexOf("NAME") == 2 ) && (argumentList.count() == 4) ){
            qDebug()<<"ADD TRAIN recognised";
            addTrainToNetwork( QString( argumentList.at(3) ) );
            currentTrain=trainList.length()-1;
            approvedCommand=true;
        }
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SET") ==1 )&& ( argumentList.indexOf("NAME") ==2 ) ){
            if (argumentList.count()==4){
                bool foundTrain=FALSE;
                foreach(Train* t, trainList){
                    if( (t->getName()) == (argumentList.at(3))) {foundTrain=TRUE; qDebug()<<"FATAL ERROR: Name is taken.";}
                }
                if(foundTrain==FALSE){
                    qDebug()<<"TRAIN SET NAME recognised with Name="<<argumentList.at(3);
                    trainList[currentTrain]->setName(argumentList.at(3));
                    approvedCommand=true;
        }   }   }
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SET") ==1 ) && ( argumentList.indexOf("CURRENT") ==2 )  && ( argumentList.indexOf("STATION") == 3 ) ){
            if (argumentList.count()==5){
                qDebug()<<"TRAIN SET STATION recognised with Station="<<argumentList.at(4)<<". Check that the station exists.";
                foreach(Station* s, stationList){
                    //QString tempstr2=argumentList.at(4)
                    if ( ( s->getName() ) == ( argumentList.at( 4 ) ) ) { trainList[currentTrain]->setCurrentStation(s->getID()); approvedCommand=true; }
        }   }   }
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SET") ==1 )&& ( argumentList.indexOf("MODEL") ==2 ) ){
            if (argumentList.count()==4){
                qDebug()<<"TRAIN SET MODEL recognised with Model="<<argumentList.at(3)<<". To be implemented!";
                approvedCommand=true;
        }   }
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SET") ==1 ) && ( argumentList.indexOf("DESIRED_SPEED") ==2 ) ){
            if (argumentList.count()==4){
                qDebug()<<"TRAIN SET DESIRED_SPEED recognised with DesiredSpeed="<<argumentList.at(3);
                bool ok=false;
                int iSpeed=argumentList.at(3).toInt(&ok,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
                if( ok==true){
                    trainList.at(currentTrain)->setDesiredSpeed(iSpeed);
                    approvedCommand=true;
        }   }   }
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("TRAVELPLAN") ==1 ) && ( argumentList.indexOf("ADD") ==2 )  && ( argumentList.indexOf("STATION") == 3 ) ){
            if (argumentList.count()==5){
                qDebug()<<"TRAIN TRAVELPLAN ADD STATION recognised with Station="<<argumentList.at(4)<<". Check that the station exists.";
                //bool stationFound=FALSE;
                foreach (Station* s, stationList) {
                    if( s->getName()==argumentList.at(4)){
                        //stationFound=TRUE;
                        trainList.at(currentTrain)->addStationToTrainRoute(s->getID());
                        approvedCommand=true;
                    }
                }
        }   }
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SELECT") ==1 ) ){
            if (argumentList.count()==3){
                foreach(Train* t, trainList){
                    if(t->getName()==argumentList.at(2)){
                        currentTrain=t->getID();
                        approvedCommand=TRUE;
                    }
                }
        }   }
        if (!approvedCommand){qDebug()<<"TC:IPN Command not recognised: "<<line;}
    }
    file.close();
}
