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
        qDebug()<<"ERROR  : Could not open file.";
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
            qDebug()<<"ERROR  : Train definition file modifies train before creating it!";
            exit(1);
        }

        /* ADD TRACK <track name> <length> */

        /* Redo code: check that length is a number. After that, check if coordinates are included.
            No coordinates: Check that there is 4 arguments, and that the fourth argument is an integer
        */
        if ( ( argumentList.indexOf("ADD") ==0 ) && ( argumentList.indexOf("TRACK") ==1 ) ) {
            if(argumentList.count()==4){
                bool ok=false;
                int iLength=argumentList.at(3).toInt(&ok,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
                if( ok==true){
                    addTrackToNetwork(argumentList.at(2), iLength);
                    approvedCommand=true;
                    currentTrack=trackList.length()-1;
                }
        } }

        /* ADD TRACK <track name> <length> COORDINATES <lat1> <lon1> <accumulated distance1>
         *  <latN> <lonN> <accumulated distanceN>
         * Note that dXXX-YYYN will have a twin eYYY-XXXS, with falling accumulated distance.*/

        /* TRACK SELECT <track name> */
        if ( ( argumentList.indexOf("TRACK") ==0 ) && ( argumentList.indexOf("SELECT") ==1 ) ){
            if (argumentList.count()==3){
                foreach(Track* t, trackList){
                    if(t->getName()==argumentList.at(2)){
                        currentTrack=t->getID();
                        approvedCommand=TRUE;
        }   }   }   }

        /* TRACK SET MAX_SPEED <speed> */
        if ( ( argumentList.indexOf("TRACK") ==0 ) && ( argumentList.indexOf("SET") ==1 ) && ( argumentList.indexOf("MAX_SPEED") ==2 )){
            if(argumentList.count()==4) {
                bool ok=false;
                int iSpeed=argumentList.at(3).toInt(&ok,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
                if( ok==true){
                    trackList.at(currentTrack)->setMaxAllowedSpeed(iSpeed);
                    approvedCommand=true;
        } } }

        /* ADD STATION <station name> [AS JUNCTION] [COORDINATES lat long] */
        if ( ( argumentList.indexOf("ADD") == 0 ) && ( argumentList.indexOf("STATION") == 1 ) ){
            bool isJunction = ( (argumentList.indexOf("AS") == 3) && ( argumentList.indexOf("JUNCTION") == 4 ) );
            bool hasCoordinates=(argumentList.indexOf("COORDINATES")!=-1); // Now, don't include coordinates to
            addStationToNetwork(argumentList.at(2),isJunction);
            qDebug() <<argumentList.at(2)<< " isJunction ? "<< isJunction << "  "<<(isJunction == true ? 5 : 3);
            if( ( argumentList.indexOf("COORDINATES") == (isJunction==true ? 5:3) ) ) {
                qDebug()<<"INFO   : Coordinates found for station "<< argumentList.at(2);

                /* LATER: Parse the latlong to the tcStation object, if necessary */
                /* Create a QML argument string */
                // 'import QtQuick 2.3; MapCircle { id: 'St\u00C3\u00A5ngby\"St\u00C3\u00A5ngby\" center { latitude:
                // 'import QtQuick 2.3;Rectangle{id:nameSample1;objectName:"nameSample1";width:40;height:40;color:"blue";x:300;y:300;}'
                //QString tmpQMLDynamicCommand = "import QtQuick 2.3; MapCircle { id: " + argumentList.at(2) +
                    ";objectName: \"" + argumentList.at(2) +
                    "Name\"; center { latitude:" + argumentList.at(isJunction == true ? 6:4) +
                    "; longitude:" + argumentList.at(isJunction == true ? 7:5 ) +
                    "}; radius:50.0; color: 'red'; opacity=0.3 ;border.width: 3}";
                //qDebug()<<tmpQMLDynamicCommand;

                createQMLStation(argumentList.at(2), isJunction, argumentList.at(isJunction ==true ? 6:4), argumentList.at(isJunction == true ? 7:5));

            }
            approvedCommand = true;
        }

        //Check if the command includes coordinates

        /* CONNECT TRACK <track name> FROM <station> TO <station> */
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
            qDebug()<<"ERROR  : Train definition file modifies train before creating it!";
            exit(1);
        }

        /* ADD TRAIN <train name> */
        if ( ( argumentList.indexOf("ADD") ==0 ) && ( argumentList.indexOf("TRAIN") ==1 ) && ( argumentList.indexOf("NAME") == 2 ) && (argumentList.count() == 4) ){
            qDebug()<<"INFO   : ADD TRAIN recognised";
            addTrainToNetwork( QString( argumentList.at(3) ) );
            currentTrain=trainList.length()-1;
            approvedCommand=true;
        }

        /* TRAIN SET <track name> <length> */
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SET") ==1 )&& ( argumentList.indexOf("NAME") ==2 ) ){
            if (argumentList.count()==4){
                bool foundTrain=FALSE;
                foreach(Train* t, trainList){
                    if( (t->getName()) == (argumentList.at(3))) {foundTrain=TRUE; qDebug()<<"FATAL ERROR: Name is taken.";}
                }
                if(foundTrain==FALSE){
                    qDebug()<<"INFO   : TRAIN SET NAME recognised with Name="<<argumentList.at(3);
                    trainList[currentTrain]->setName(argumentList.at(3));
                    approvedCommand=true;
        }   }   }

        /* TRAIN SET STATION <station name> */
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SET") ==1 ) && ( argumentList.indexOf("CURRENT") ==2 )  && ( argumentList.indexOf("STATION") == 3 ) ){
            if (argumentList.count()==5){
                qDebug()<<"INFO   : TRAIN SET STATION recognised with Station="<<argumentList.at(4)<<". Check that the station exists.";
                foreach(Station* s, stationList){
                    //QString tempstr2=argumentList.at(4)
                    if ( ( s->getName() ) == ( argumentList.at( 4 ) ) ) { trainList[currentTrain]->setCurrentStation(s->getID()); approvedCommand=true; }
        }   }   }

        /* TRAIN SET MODEL <model type> */
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SET") ==1 )&& ( argumentList.indexOf("MODEL") ==2 ) ){
            if (argumentList.count()==4){
                qDebug()<<"INFO   : TRAIN SET MODEL recognised with Model="<<argumentList.at(3)<<". To be implemented!";
                approvedCommand=true;
        }   }

        /* TRAIN SET DESIRED_SPEED <desired speed> */
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SET") ==1 )
                && ( argumentList.indexOf("DESIRED_SPEED") ==2 ) ){
            if (argumentList.count()==4){
                qDebug()<<"INFO   : TRAIN SET DESIRED_SPEED recognised with DesiredSpeed="<<argumentList.at(3);
                bool ok=false;
                int iSpeed=argumentList.at(3).toInt(&ok,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
                if( ok==true){
                    trainList.at(currentTrain)->setDesiredSpeed(iSpeed);
                    approvedCommand=true;
        }   }   }

        /* ADD TRACK <track name> <length> */
        if ( ( argumentList.indexOf("TRAIN") == 0 ) && ( argumentList.indexOf("TRAVELPLAN") ==1 )
                && ( argumentList.indexOf("ADD") == 2 ) && ( argumentList.indexOf("STATION") == 3 ) ){
            if (argumentList.count()==5){
                qDebug()<<"INFO   : TRAIN TRAVELPLAN ADD STATION recognised with Station="<<argumentList.at(4)<<". TODO: Check that the station exists.";
                //bool stationFound=FALSE;
                foreach (Station* s, stationList) {
                    if( s->getName()==argumentList.at(4)){
                        //stationFound=TRUE;
                        trainList.at(currentTrain)->addStationToTrainRoute(s->getID());
                        approvedCommand=true;
        }    }   }   }

        /* TRAIN SELECT <train name name> */
        if ( ( argumentList.indexOf("TRAIN") ==0 ) && ( argumentList.indexOf("SELECT") ==1 ) ){
            if (argumentList.count()==3){
                foreach(Train* t, trainList){
                    if(t->getName()==argumentList.at(2)){
                        currentTrain=t->getID();
                        approvedCommand=TRUE;
                    }
                }
        }   }
        if (!approvedCommand){qDebug()<<"ERROR  : TC:IPN Command not recognised: "<<line;}
    }
    file.close();

    //This method will be called with care, in order to reduce the cost for updating the tables
    ui->trainListTableView->resizeColumnsToContents();
    ui->trackListTableView->resizeColumnsToContents();
    ui->stationListTableView->resizeColumnsToContents();
}
