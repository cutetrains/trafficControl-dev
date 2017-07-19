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
#include "../inc/TrafficControl.h"
#include "ui_TrafficControl.h"
#define TRACK 1//Replace with constants that are global!
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1
using namespace std;

/*!
 * This method imports a network from a file to the program.
 * @todo: If this part becomes too extensive, place this code in a separate source file.
 * @todo: Introduce consistensy checks for files.
 */
void TrafficControl::importPredefinedNetwork()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open File"),
                                                  "C://temp//train//",
                                                  tr("Files (*.*)"));
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug()<<"ERROR  : Could not open file.";
  }

  int currentTrain = UNDEFINED;
  int currentTrack = UNDEFINED;

  QTextStream in(&file);
  while(!in.atEnd())
  {
    QString line = in.readLine();
    QStringList argumentList=line.split(" ");
    bool approvedCommand = false;

    //Code for analysing track commands
    if ((0 == argumentList.indexOf("TRACK")) &&
        (UNDEFINED == currentTrack)){
      qDebug()<<"ERROR  : Train definition file modifies train before creating it!";
      exit(1);
    }

    /* ADD TRACK <track name> <length> [COORDINATES lat long]
     * Redo code: check that length is a number. After that, check if coordinates are included.
     * No coordinates: Check that there is 4 arguments, and that the fourth argument is an integer
     * Note that dXXX-YYYN will have a twin eYYY-XXXS, with falling accumulated distance.
     * The python script will need to add the second track four meters away.*/
    if ((0 == argumentList.indexOf("ADD")) &&
        (1 == argumentList.indexOf("TRACK"))) {
      bool isLengthInt = false;
      int iLength = argumentList.at(3).toInt(&isLengthInt, 10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
      if (true == isLengthInt){
        if ( 4 == argumentList.count() )
        {
          QStringList emptyCoordinates;
          addTrackToNetwork(argumentList.at(2), iLength, emptyCoordinates);
          approvedCommand = true;
          currentTrack = trackList.length()-1;
        }
        else if (4 == argumentList.indexOf("COORDINATES")  &&
                 ( argumentList.count()%2 == 1 ) ){
          addTrackToNetwork(argumentList.at(2), iLength, argumentList.mid(5,-1));
          approvedCommand = true;
          currentTrack = trackList.length()-1;
    } } }

    /* TRACK SELECT <track name> */
    if ((0 == argumentList.indexOf("TRACK")) &&
        (1 == argumentList.indexOf("SELECT")) &&
        (3 == argumentList.count())){
      foreach(Track* t, trackList){
        if(t->getName() == argumentList.at(2)){
          currentTrack = t->getID();
          approvedCommand = true;
    } } }

    /* TRACK SET MAX_SPEED <speed> */
    if ((0 == argumentList.indexOf("TRACK")) &&
        (1 == argumentList.indexOf("SET")) &&
        (2 == argumentList.indexOf("MAX_SPEED")) &&
        (4 == argumentList.count())) {
      bool speedIsInt = true;
      int iSpeed=argumentList.at(3).toInt(&speedIsInt,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
      if (true == speedIsInt){
        trackList.at(currentTrack)->setMaxAllowedSpeed(iSpeed);

      }
      approvedCommand = true;
    }

    /* ADD STATION <station name> [AS JUNCTION] [COORDINATES lat long] */
    if ((0 == argumentList.indexOf("ADD")) &&
        (1 == argumentList.indexOf("STATION"))){
      bool isJunction = ((3 == argumentList.indexOf("AS")) &&
                         (4 == argumentList.indexOf("JUNCTION")));
      if ((argumentList.indexOf("COORDINATES") == (isJunction==true ? 5 : 3) ) ) {
        addStationToNetwork(argumentList.at(2),
                            isJunction,
                            argumentList.at(true == isJunction ? 6 : 4),
                            argumentList.at(true == isJunction ? 7 : 5));
      } else {
        addStationToNetwork(argumentList.at(2),
                            isJunction,
                            "",
                            "");
      }
      approvedCommand = true;
    }

    /* CONNECT TRACK <track name> FROM <station> TO <station> */
    if ((0 == argumentList.indexOf("CONNECT")) &&
        (1 == argumentList.indexOf("TRACK")) &&
        (3 == argumentList.indexOf("FROM")) &&
        (5 == argumentList.indexOf("TO")) &&
        (7 == argumentList.count()) ){
        //Check that the second argument reflects an existing track, and that the stations exist.
      int foundTrack = UNDEFINED;
      //qDebug()<<"INFO   : CONNECT TRACK name FROM station TO station recognised";
      foreach(Track* t, trackList){
        if (t->getName()==argumentList.at(2)) {foundTrack=t->getID();
      }
      int foundStartStation = UNDEFINED;
      int foundEndStation = UNDEFINED;
      foreach(Station* s, stationList){
        if(s->getName()==argumentList.at(4)){
          foundStartStation=s->getID();
        }
        if(s->getName()==argumentList.at(6)){
          foundEndStation=s->getID();
        }
      }
      if ((foundStartStation != UNDEFINED) &&
          (foundEndStation != UNDEFINED) &&
          (foundStartStation != foundEndStation) &&
          (foundTrack != UNDEFINED)){
        connectTrackToStations(argumentList.at(2),
                               argumentList.at(4),
                               argumentList.at(6));
        approvedCommand=true;
    } } }

    //Code for analysing train commands
    if ((0 == argumentList.indexOf("TRAIN")) &&
        (UNDEFINED == currentTrain)){
      qDebug()<<"ERROR  : Train definition file modifies train before creating it!";
      exit(1);
    }

    /* ADD TRAIN NAME <train name> */
    if ((0 == argumentList.indexOf("ADD")) &&
        (1 == argumentList.indexOf("TRAIN")) &&
        (2 == argumentList.indexOf("NAME")) &&
        (4 == argumentList.count())){
      //qDebug()<<"INFO   : ADD TRAIN recognised";
      addTrainToNetwork(QString(argumentList.at(3)));
      currentTrain=trainList.length()-1;
      approvedCommand=true;
    }

    /* TRAIN SET <track name> <length> */
    if ((0 == argumentList.indexOf("TRAIN")) &&
        (1 == argumentList.indexOf("SET")) &&
        (2 == argumentList.indexOf("NAME")) &&
        (4 == argumentList.count())){
      bool foundTrain = false;
      foreach (Train* t, trainList){
        if (t->getName() == argumentList.at(3)) {
          foundTrain = true;
          qDebug()<<"FATAL ERROR: Name is taken.";
        }
      }
      if (false == foundTrain){
        //qDebug()<<"INFO   : TRAIN SET NAME recognised with Name="<<argumentList.at(3);
        trainList[currentTrain]->setName(argumentList.at(3));
        approvedCommand=true;
      } else {
        qDebug()<<"ERROR  : TRAIN SET NAME, name alredy taken:"<<argumentList.at(3);
      }
    }

    /* TRAIN SET STATION <station name> */
    if ((0 == argumentList.indexOf("TRAIN")) &&
        (1 == argumentList.indexOf("SET")) &&
        (2 == argumentList.indexOf("CURRENT")) &&
        (3 == argumentList.indexOf("STATION")) &&
        (5 == argumentList.count())){
      //qDebug()<<"INFO   : TRAIN SET STATION recognised with Station="<<argumentList.at(4)<<". Check that the station exists.";
      foreach(Station* s, stationList){
        //QString tempstr2=argumentList.at(4)
        if (s->getName() == argumentList.at(4)){
          trainList[currentTrain]->setCurrentStation(s->getID());
          approvedCommand=true;
    } } }

    /* TRAIN SET MODEL <model type> */
    if ((0 == argumentList.indexOf("TRAIN")) &&
        (1 == argumentList.indexOf("SET")) &&
        (2 == argumentList.indexOf("MODEL")) &&
        (4 == argumentList.count())){
      //qDebug()<<"INFO   : TRAIN SET MODEL recognised with Model="<<argumentList.at(3)<<". To be implemented!";
      approvedCommand=true;
    }

    /* TRAIN SET DESIRED SPEED <desired speed> */
    if ((0 == argumentList.indexOf("TRAIN")) &&
        (1 == argumentList.indexOf("SET")) &&
        (2 == argumentList.indexOf("DESIRED")) &&
        (3 == argumentList.indexOf("SPEED")) &&
        (5 == argumentList.count())){
      //qDebug()<<"INFO   : TRAIN SET DESIRED SPEED recognised with DesiredSpeed="<<argumentList.at(4);
      bool isSpeedInt=false;
      int iSpeed=argumentList.at(4).toInt(&isSpeedInt,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
      if (true == isSpeedInt){
        trainList.at(currentTrain)->setDesiredSpeed(iSpeed);
        approvedCommand=true;
    } }

    /* ADD TRACK <track name> <length> */
    if ((0 == argumentList.indexOf("TRAIN")) &&
        (1 == argumentList.indexOf("TRAVELPLAN")) &&
        (2 == argumentList.indexOf("ADD")) &&
        (3 == argumentList.indexOf("STATION")) &&
        (5 == argumentList.count())){
      //qDebug()<<"INFO   : TRAIN TRAVELPLAN ADD TRACK recognised with Track="
      //        <<argumentList.at(4)<<". TODO: Check that the station exists.";
      foreach (Station* s, stationList) {
        if( s->getName()==argumentList.at(4)){
      //    qDebug()<<"IMPORT : "<<argumentList.at(4) <<" found. Adding to travelplan";
          trainList.at(currentTrain)->addStationToTravelPlan(s->getID());
          approvedCommand=true;
        }
      }
    }

    /* TRAIN SELECT <train name name> */
    if ((0 == argumentList.indexOf("TRAIN")) &&
        (1 == argumentList.indexOf("SELECT")) &&
        (3 == argumentList.count())){
      foreach(Train* t, trainList){
        if(t->getName()==argumentList.at(2)){
          currentTrain=t->getID();
          approvedCommand=true;
        }
      }
    }

    if (!approvedCommand){
      qDebug()<<"ERROR  : TC:IPN Command not recognised: "<<line;
    }
  }
  /*if(handleQMLObject != NULL) {
    handleQMLObject->dumpObjectTree();
  }*/
  file.close();

  //This method will be called with care, in order to reduce the cost for updating the tables
  ui->trainListTableView->resizeColumnsToContents();
  ui->trackListTableView->resizeColumnsToContents();
  ui->stationListTableView->resizeColumnsToContents();
}
