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
#include <QRegularExpression>
#include "../inc/tcnetworkcontrol.h"
//#include "ui_trafficcontrol.h"
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
 * @todo: Use Regular Expressions and redo code
 * @todo: Introduce consistensy checks for files.
 */
bool NetworkControl::parseCmd(QString inputLine)
{
    QStringList argumentList=inputLine.split(" ");
    bool approvedCommand = false;

    QStringList reResults;
    QRegularExpression re;
    QRegularExpressionMatch match;

    //Code for analysing track commands
    if ((0 == argumentList.indexOf("TRACK")) &&
            (UNDEFINED == cmdParserCurrentTrack)){
        qDebug()<<"ERROR  : Train definition file modifies train before creating it!";
        return false;
    }

    //Code for analysing train commands
    if ((0 == argumentList.indexOf("TRAIN")) &&
            (UNDEFINED == cmdParserCurrentTrain)){
        qDebug()<<"ERROR  : Train definition file modifies train before creating it!";
        return false;
    }

    re.setPattern("^ADD (STATION|JUNCTION) ([a-zA-Z0-9_]*) ?(COORDINATES)? ?(-?[0-9]*.[0-9]* -?[0-9]*.[0-9]*)?");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        if(3 == reResults.length()){//Coordinates do not exist
            addStationToNetwork(reResults.at(2),
                                QString::compare(reResults.at(1), "JUNCTION", Qt::CaseInsensitive) == 0 ? true: false,
                                "", "");
            approvedCommand = true;
        } else if (5 == reResults.length()) { // Coordinates
            QStringList coordinates = reResults.at(4).split(" ");
            addStationToNetwork(reResults.at(2),
                                QString::compare(reResults.at(1), "JUNCTION", Qt::CaseInsensitive) == 0 ? true: false,
                                coordinates.at(0), coordinates.at(1));
            approvedCommand = true;
        }
        else {qDebug()<<"COORDINATE ERROR "<<inputLine;}
    }

    re.setPattern("^ADD TRACK ([a-zA-Z0-9_]*) ([0-9]*) ?(COORDINATES)?( -?[0-9]+.[0-9]+)*$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        //qDebug()<<reResults;
        QStringList coordinateList = reResults.at(0).split(" ");
        double coordinate;
        bool ok = true;

        if(coordinateList.length()==4){
            QStringList emptyCoordinates;
            approvedCommand = addTrackToNetwork(argumentList.at(2), argumentList.at(3).toInt(), emptyCoordinates);
        }
        else if( (coordinateList.length() % 2) == 1 && coordinateList.length() >5) {
            for(int iii=5; iii<coordinateList.length();iii++) {
                coordinate = coordinateList.at(iii).toFloat(&ok);
                if(!ok){break;}
                if(iii%2 == 1) {
                    if(abs(coordinate)>90) {
                        qDebug()<<"coordinate too big (90): "<<coordinate;
                        ok = false;
                        break;
                    }
                } else {
                    if(abs(coordinate) > 180) {
                        qDebug()<<"coordinate too big (180): "<<coordinate;
                        ok = false;
                        break;
            }   }   }
            if(ok){
                approvedCommand = addTrackToNetwork(coordinateList.at(2), coordinateList.at(3).toInt(), argumentList.mid(5,-1));
                return true;
            } else {
                return false;
            }
        }
    }

    re.setPattern("^TRACK SELECT ([a-zA-Z0-9_]*)");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        foreach(Track* t, trackList){
            if(t->getName() == reResults.at(0)){
                cmdParserCurrentTrack = t->getID();
                approvedCommand = true;
            } } }

    re.setPattern("^TRACK SET MAX SPEED ([0-9]*)");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        bool speedIsInt = true;
        int iSpeed=reResults.at(0).toInt(&speedIsInt,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
        if (true == speedIsInt){
            trackList.at(cmdParserCurrentTrack)->setMaxAllowedSpeed(iSpeed);
        }
        approvedCommand = true;
    }

    re.setPattern("^CONNECT TRACK ([a-zA-Z0-9_]*) FROM ([a-zA-Z0-9_]*) TO ([a-zA-Z0-9_]*)$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        int foundTrack = UNDEFINED;
        foreach(Track* t, trackList) {
            if (t->getName()==reResults.at(1)) { foundTrack=t->getID(); }
        }
        int foundStartStation = UNDEFINED;
        int foundEndStation = UNDEFINED;
        foreach(Station* s, stationList) {
            if(s->getName()==reResults.at(2)) { foundStartStation=s->getID(); }
            if(s->getName()==reResults.at(3)) { foundEndStation=s->getID(); }
        }
        if ((foundStartStation != UNDEFINED) &&
                (foundEndStation != UNDEFINED) &&
                (foundStartStation != foundEndStation) &&
                (foundTrack != UNDEFINED)) {
            connectTrackToStationsByName(reResults.at(1),
                                         reResults.at(2),
                                         reResults.at(3));
            approvedCommand=true;
        } }

    re.setPattern("^ADD TRAIN ([a-zA-Z0-9_]*)$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        addTrainToNetwork(reResults.at(1));
        cmdParserCurrentTrain=trainList.length()-1;
        approvedCommand=true;
    }

    re.setPattern("^TRAIN SET NAME ([a-zA-Z0-9_]*)$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        bool foundTrain = false;
        foreach (Train* t, trainList){
            if (t->getName() == reResults.at(1)) {
                foundTrain = true;
                qDebug()<<"FATAL ERROR: Name is taken.";
            }
        }
        if (false == foundTrain){
            //qDebug()<<"INFO   : TRAIN SET NAME recognised with Name="<<argumentList.at(3);
            trainList[cmdParserCurrentTrain]->setName(reResults.at(1));
            approvedCommand=true;
        } else {
            qDebug()<<"ERROR  : TRAIN SET NAME, name alredy taken:"<<reResults.at(1);
        }
    }

    re.setPattern("^TRAIN SET CURRENT STATION ([a-zA-Z0-9_]*)$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        foreach(Station* s, stationList){
            if (s->getName() == reResults.at(1)){
                trainList[cmdParserCurrentTrain]->setCurrentStation(s->getID());
                approvedCommand=true;
    } } }

    re.setPattern("^TRAIN SET MODEL ([a-zA-Z0-9_]*)$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        qDebug()<<"INFO   : TRAIN SET MODEL recognised with Model="<<reResults.at(1)<<". To be implemented!";
        approvedCommand=true;
    }

    re.setPattern("^TRAIN SET DESIRED SPEED ([0-9]*)$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        bool isSpeedInt=false;
        int iSpeed=reResults.at(1).toInt(&isSpeedInt,10) ;// THIS CHECK AT OTHER PLACES TOO!!!!
        if (true == isSpeedInt){
            trainList.at(cmdParserCurrentTrain)->setDesiredSpeed(iSpeed);
            approvedCommand=true;
        } }

    re.setPattern("^TRAIN TRAVELPLAN ADD STATION ([a-zA-Z0-9_]*)$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        foreach (Station* s, stationList) {
            if( s->getName()==reResults.at(1)){;
                trainList.at(cmdParserCurrentTrain)->addStationToTravelPlan(s->getID());
                approvedCommand=true;
    } } }

    re.setPattern("^TRAIN SELECT ([a-zA-Z0-9_]*)$");
    match = re.match(inputLine);
    if (match.hasMatch()){
        reResults = match.capturedTexts();
        foreach(Train* t, trainList){
            if(t->getName()==reResults.at(1)){
                cmdParserCurrentTrain=t->getID();
                approvedCommand=true;
    }  }  }

    if (!approvedCommand){
        qDebug()<<"ERROR  : TC:IPN Command not recognised: "<<inputLine;
    }
    return approvedCommand;
}
