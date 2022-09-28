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
#include <QString>
#include <QTextStream>
#include <QFileDialog>
#include "../inc/tcNetworkControl.h"
#include "ui_TrafficControl.h"
#define TRACK 1//Replace with constants that are global!
#define TRAIN 2
#define STATION 3
#define UNDEFINED -1
using namespace std;

/*!
 * This method imports a list of network commands from a file to the program.
 *
 * @param inputLine One network commands as a QStringList.
 *
 * @return Success or Failure
 */
bool NetworkControl::parseMultipleNetworkCommand(QStringList inputLines)
{
  bool result = true;

  foreach(QString thisString, inputLines)
  {
    qDebug()<<thisString;
    result = result && parseCmd(thisString);
  }
  return result;
}

/*!
 * This method imports a network command from a file to the program.
 *
 * @param inputLine One network command.
 *
 * @return Success or Failure
 *
 * @todo: If this part becomes too extensive, place this code in a separate source file.
 * @todo: Use Regular Expressions
 * @todo: Introduce consistensy checks for files.
 */
bool NetworkControl::parseCmd(QString inputLine)
{
  QStringList argumentList=inputLine.split(" ");
  bool approvedCommand = false;

  //Code for analysing track commands
  if ((0 == argumentList.indexOf("TRACK")) &&
      (UNDEFINED == cmdParserCurrentTrack)){
    qDebug()<<"ERROR  : Train definition file modifies train before creating it!";
    return false;
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
        approvedCommand = addTrackToNetwork(argumentList.at(2), iLength, emptyCoordinates);
      }
      else if (4 == argumentList.indexOf("COORDINATES")  &&
             ( argumentList.count()%2 == 1 ) ){
        bool ok;
        float coordinate;
        for(int iii=5; iii<argumentList.length();iii++)
        {
          coordinate = argumentList.at(iii).toFloat(&ok);
          if(!ok){break;}
          if(iii%2 == 0)
          {
            if(abs(coordinate) > 180)
            {
              ok = false;
              break;
            }
          } else {
            if(abs(coordinate)>90)
            {
              ok = false;
              break;
            }
          }
        }
        if(ok){
          approvedCommand = addTrackToNetwork(argumentList.at(2), iLength, argumentList.mid(5,-1));
          return true;
        } else {
          return false;
        }
      }
      if(approvedCommand) {
        cmdParserCurrentTrack = trackList.length()-1;
      }
  } }

  /* TRACK SELECT <track name> */
  if ((0 == argumentList.indexOf("TRACK")) &&
      (1 == argumentList.indexOf("SELECT")) &&
      (3 == argumentList.count())){
    foreach(Track* t, trackList){
      if(t->getName() == argumentList.at(2)){
        cmdParserCurrentTrack = t->getID();
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
      trackList.at(cmdParserCurrentTrack)->setMaxAllowedSpeed(iSpeed);
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
      (7 == argumentList.count()) ) {
      //Check that the second argument reflects an existing track, and that the stations exist.
    int foundTrack = UNDEFINED;
    foreach(Track* t, trackList) {
      if (t->getName()==argumentList.at(2)) { foundTrack=t->getID(); }
    }
    int foundStartStation = UNDEFINED;
    int foundEndStation = UNDEFINED;
    foreach(Station* s, stationList) {
      if(s->getName()==argumentList.at(4)) { foundStartStation=s->getID(); }
      if(s->getName()==argumentList.at(6)) { foundEndStation=s->getID(); }
    }
    if ((foundStartStation != UNDEFINED) &&
        (foundEndStation != UNDEFINED) &&
        (foundStartStation != foundEndStation) &&
        (foundTrack != UNDEFINED)) {
      connectTrackToStationsByName(argumentList.at(2),
                                   argumentList.at(4),
                                   argumentList.at(6));
      approvedCommand=true;
  } }

  //Code for analysing train commands
  if ((0 == argumentList.indexOf("TRAIN")) &&
      (UNDEFINED == cmdParserCurrentTrain)){
    qDebug()<<"ERROR  : Train definition file modifies train before creating it!";
    exit(1);
  }

  /* ADD TRAIN <train name> */
  if ((0 == argumentList.indexOf("ADD")) &&
      (1 == argumentList.indexOf("TRAIN")) &&
      (3 == argumentList.count())){
    //qDebug()<<"INFO   : ADD TRAIN recognised";
    addTrainToNetwork(QString(argumentList.at(2)));
    cmdParserCurrentTrain=trainList.length()-1;
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
      trainList[cmdParserCurrentTrain]->setName(argumentList.at(3));
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
      if (s->getName() == argumentList.at(4)){
        trainList[cmdParserCurrentTrain]->setCurrentStation(s->getID());
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
          trainList.at(cmdParserCurrentTrain)->setDesiredSpeed(iSpeed);
          approvedCommand=true;
      } }

  /* TRAIN TRAVELPLAN ADD STATION <track name> <length> */
  if ((0 == argumentList.indexOf("TRAIN")) &&
          (1 == argumentList.indexOf("TRAVELPLAN")) &&
          (2 == argumentList.indexOf("ADD")) &&
          (3 == argumentList.indexOf("STATION")) &&
          (5 == argumentList.count())){
      foreach (Station* s, stationList) {
          if( s->getName()==argumentList.at(4)){
              //qDebug()<<"IMPORT : "<<argumentList.at(4) <<" found. Adding to travelplan";
              trainList.at(cmdParserCurrentTrain)->addStationToTravelPlan(s->getID());
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
              cmdParserCurrentTrain=t->getID();
              approvedCommand=true;
          }
      }
  }
  if (!approvedCommand){
      qDebug()<<"ERROR  : TC:IPN Command not recognised: "<<inputLine;
  }
  return approvedCommand;
}
