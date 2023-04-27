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

#include "../inc/networkdesigner.h"

NetworkDesigner::NetworkDesigner(QObject *parent) : QObject(parent)
{
}

/*!
 * Updates the TrafficNetworkMap textbox in user interface.
 * TNM is generated from KML file using KML parser.
 * This is using a python command interpreter.
 * TODO: Make this OS-agnostic.
 * TODO: Investigate if it is possible to use python from within the program, without relying on a file on the local file system.
 *
 * @param tnmFile the generated TrafficNetworkMap string
 */
bool NetworkDesigner::convertKmlToTnm(QString kmlFileName)
{
  /*
  //TODO: Make os-agnostic
  QString  program( "python.exe " );
  QProcess p;
  QStringList args = QStringList();
  //TODO: Relies on GIT, move to resources instead?
  args << "C:/Users/gusta/GIT/trafficControl-dev/scripts/KMLParser/TrackStationConnector.py";
  args << kmlFileName;
  p.start(program,args);
  p.waitForFinished(-1);
  QString trafficNetworkMapFile = p.readAll();
  */
    int trackLength;

    QRegularExpressionMatch match;

    TrackListItem trackListItem;
    QList<TrackListItem> trackList;
    StationListItem stationListItem;
    QList<StationListItem> stationList;
    enum class PlaceMarkType {none, track, doubleTrack, station, junction};
    QList<QGeoCoordinate> coordinateList;
    QList<QGeoCoordinate> reversedCoordinateList;
    PlaceMarkType pmType = PlaceMarkType::none;
    double lonShift = 0.0001;
    double latShift = 0.0001;
    QGeoCoordinate thisCoordinate;
    QString cmdString = "";
    double maxAllowedDistance = 200.0;

    QDomDocument doc("mydocument");
    QDomElement e;
    QDomNodeList plmName;
    QDomNodeList plmCoordinates;

    QString thisName;
    QString thisCoordinateRaw;
    QStringList rawCoordinateList;
    QGeoCoordinate previousCoordinate;
    QGeoCoordinate nextCoordinate;
    //QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    //QString fileLocation = homeLocation+"/train/NC_Skane.kml";
    QString directions ="";
    QFile file(kmlFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }

    QString outFile;
    /*QFile outFile("out.txt");
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&outFile);
    */
    QDomElement docElem = doc.documentElement();
    QDomNodeList pml = docElem.elementsByTagName("Placemark");
    QRegularExpression reJunction("^J[A-Z]");
    QRegularExpression reDouble("^d[A-Z]");
    for(int iii = 0; iii < pml.length(); iii++){
        coordinateList.clear();
        plmName = pml.at(iii).toElement().elementsByTagName("name");
        thisName = TraverseXmlNode(plmName.at(0)).replace("-","_");
        //outFile.append(":::"+thisName+":::"+thisName.length());
        plmCoordinates = pml.at(iii).toElement().elementsByTagName("coordinates");
        thisCoordinateRaw = TraverseXmlNode(plmCoordinates.at(0)).replace("\n", "").replace("\t","");
        rawCoordinateList = thisCoordinateRaw.split(" ");
        if(rawCoordinateList.length()>1){
            match = reDouble.match(thisName);
            if(match.hasMatch()){
                pmType = PlaceMarkType::doubleTrack;
            } else {
                pmType = PlaceMarkType::track;
            }
        } else {
            match = reJunction.match(thisName);
            if(match.hasMatch()){
                 pmType = PlaceMarkType::junction;
            } else{
                 pmType = PlaceMarkType::station;
            }
        }

        previousCoordinate = QGeoCoordinate(100.0,100.0);//Intentionally invalid the first time
        trackLength = 0.0;
        foreach (QString thisCoordinateString, rawCoordinateList) {
            QStringList thisCoordinateSplitted = thisCoordinateString.split(",");
            if(thisCoordinateSplitted.length()== 3)//empty sometimes
            {
                nextCoordinate = QGeoCoordinate(thisCoordinateSplitted.at(1).toDouble(),
                                                thisCoordinateSplitted.at(0).toDouble());
                if(previousCoordinate.isValid() == true){
                    trackLength = trackLength + previousCoordinate.distanceTo(nextCoordinate);
                }
                previousCoordinate.setLatitude(nextCoordinate.latitude());
                previousCoordinate.setLongitude(nextCoordinate.longitude());
                coordinateList << nextCoordinate;
            }
        }

        if(PlaceMarkType::doubleTrack == pmType){
          double startLat = coordinateList.at(0).latitude();
          double startLon = coordinateList.at(0).longitude();
          double endLat = coordinateList.at(coordinateList.length()-1).latitude();
          double endLon = coordinateList.at(coordinateList.length()-1).longitude();

          lonShift = 0.0005 * (endLon - startLon) / (sqrt(qPow(endLon-startLon,2) + qPow(endLat-startLat,2)));
          latShift = 0.0005 * (endLat - startLat) / (sqrt(qPow(endLon-startLon,2) + qPow(endLat-startLat,2)));

          if(abs(lonShift)<abs(latShift)){//S or N
              if(latShift > 0) { directions = "NS"; } else { directions = "SN"; }
          }else{ //W or E
              if(lonShift>0){ directions = "EW"; } else { directions = "WE"; }
          }

          reversedCoordinateList.clear();
          for(int iii = coordinateList.length()-1; iii>= 0; iii--){
            thisCoordinate.setLatitude(coordinateList.at(iii).latitude()+latShift);
            thisCoordinate.setLongitude(coordinateList.at(iii).longitude()+lonShift);
            reversedCoordinateList.append(thisCoordinate);
          }
        }

        trackListItem.coordinates.clear();
        cmdString = "";
        switch(pmType)
        {
          case PlaceMarkType::junction:
            {
              cmdString.append("ADD JUNCTION ");
              [[fallthrough]];
            }
          case PlaceMarkType::station:
            {
              if(cmdString.isEmpty()){
                  cmdString.append("ADD STATION ");
              }
              stationListItem.name = thisName.toLatin1();//.replace("\"", "");
              stationListItem.isJunction = false;
              stationListItem.coordinates = coordinateList.at(0);
              stationList<<stationListItem;
              outFile.append(cmdString + stationListItem.name + " COORDINATES "
                + QString::number(stationListItem.coordinates.latitude(), 'g', 8) +" "
                + QString::number(stationListItem.coordinates.longitude(), 'g', 8)+'\n');
              break;
            }
          case PlaceMarkType::track:{
            QString thisCoordinateListToPrint = "";
            trackListItem.name =  thisName.replace("\"","");
            foreach(QGeoCoordinate gc, coordinateList){
                trackListItem.coordinates <<gc;
                thisCoordinateListToPrint.append(QString::number(gc.latitude(), 'g', 8)  +" "+QString::number(gc.longitude(), 'g', 8)  +" ");
            }
            if(coordinateList.length()>0){thisCoordinateListToPrint.chop(1);}
            trackListItem.length = trackLength;
            trackList<<trackListItem;
            outFile.append("ADD TRACK " + trackListItem.name + " " + QString::number(trackListItem.length)  + " COORDINATES " + thisCoordinateListToPrint + '\n');
            break;
          }
          case PlaceMarkType::doubleTrack:{
            QString thisCoordinateListToPrint = "";
            trackListItem.name =  thisName.replace("\"","");
            trackListItem.name.append("_").append(directions.at(0));
            foreach(QGeoCoordinate gc, coordinateList){
                trackListItem.coordinates <<gc;
                thisCoordinateListToPrint.append(QString::number(gc.latitude(), 'g', 8)  +" "+QString::number(gc.longitude(), 'g', 8)  +" ");
            }
            trackListItem.length = trackLength;
            trackList<<trackListItem;
            if(coordinateList.length()>0){thisCoordinateListToPrint.chop(1);}
            outFile.append("ADD TRACK " + trackListItem.name + " "+ QString::number(trackListItem.length) + " COORDINATES " + thisCoordinateListToPrint.trimmed() + '\n');
            trackListItem.name =  thisName.replace("\"","");
            trackListItem.name.append("_").append(directions.at(1)); //REVERSE
            trackListItem.coordinates.clear();
            thisCoordinateListToPrint ="";
            foreach(QGeoCoordinate gc, reversedCoordinateList){
                trackListItem.coordinates <<gc;
                thisCoordinateListToPrint.append(QString::number(gc.latitude(), 'g', 8)  +" "+QString::number(gc.longitude(), 'g', 8)  +" ");
            }
            if(reversedCoordinateList.length()>0){thisCoordinateListToPrint.chop(1);}
            trackList<<trackListItem;
            outFile.append("ADD TRACK " + trackListItem.name + " " + QString::number(trackListItem.length) + " COORDINATES " + thisCoordinateListToPrint.trimmed() + '\n');
            break;
          }
          case PlaceMarkType::none:{
            break;
          }
        }
    }

    for(int ttt = 0; ttt< trackList.length();ttt++)//Connect tracks to statiosn
    {
        int closestStartIndex = -1;
        int closestEndIndex = -1;

        double closestStartDistance = 1000000000.0;
        double closestEndDistance = 1000000000.0;
        double thisDistance = 0;//distanceTo
        for(int sss = 0;sss<stationList.length();sss++)
        {
            thisDistance = stationList.at(sss).coordinates.distanceTo(trackList.at(ttt).coordinates.first());
            if(thisDistance < closestStartDistance){
                closestStartIndex = sss;
                closestStartDistance = thisDistance;
            }
            //out<<"Distance between "<< stationList.at(sss).name <<" and start of "<<trackList.at(ttt).name<< "is "<< thisDistance<< " meters."<<'\n';
            thisDistance = stationList.at(sss).coordinates.distanceTo(trackList.at(ttt).coordinates.last());
            if(thisDistance < closestEndDistance){
                closestEndIndex = sss;
                closestEndDistance = thisDistance;
            }
        }
        if(closestStartDistance < maxAllowedDistance && closestEndDistance < maxAllowedDistance){
            outFile.append("CONNECT TRACK " + trackList.at(ttt).name + " FROM " + stationList.at(closestStartIndex).name + " TO " +
                           stationList.at(closestEndIndex).name +'\n');
            outFile.append("CONNECT TRACK " + trackList.at(ttt).name + " FROM " + stationList.at(closestEndIndex).name + " TO " +
                           stationList.at(closestStartIndex).name +'\n');
        } else {
            qDebug()<<"ERROR - too long distance from station. Closest start distance for " + trackList.at(ttt).name + " is " +
                           closestStartDistance + " " + stationList.at(closestStartIndex).name + ". Closest end distance for " +
                           trackList.at(ttt).name + " is " + closestEndDistance + " " + stationList.at(closestEndIndex).name + '\n';
        }
    }

  //emit kmlToTnmConversionDone(trafficNetworkMapFile);
    emit kmlToTnmConversionDone(outFile);
  //TODO: ANALYSE TNM OUTPUT AND SEND REPORT TO ERROR, ALONG WITH BOOLEAN. THIS CAN BE ADDED TO THE EXISTING SIGNAL
    return false;
}

bool NetworkDesigner::importTnmOrTnf(QString tnmOrTnfFileName)
{
    QFile file(tnmOrTnfFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qDebug()<<"ERROR  : Could not open file.";
    }

    QTextStream in(&file);
    QStringList fileStrings;
    while(!in.atEnd())
    {
      fileStrings << in.readLine();
    }
    file.close();
    //TODO: THIS MAY NEED TO BE MODIFIED IF THERE IS ERROR HANDLING!!!
    emit kmlToTnmConversionDone(fileStrings.join("\r\n"));
    return false;
}

bool NetworkDesigner::importTno(QString tnoFileName)
{
    QFile file(tnoFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qDebug()<<"ERROR  : Could not open file.";
      return false;
    }

    QTextStream in(&file);
    QStringList fileStrings;
    while(!in.atEnd())
    {
      fileStrings << in.readLine();
    }
    file.close();
    emit tnoFileOpened(fileStrings.join("\r\n"));

    return true;
}

//https://stackoverflow.com/questions/15882786/qt-c-qdomdocument-recursively-iterating-over-xml-data-retrieving-text-elemen
QString NetworkDesigner::TraverseXmlNode(const QDomNode& node)
{
  QDomNode domNode = node.firstChild();
  QDomElement domElement;
  QDomText domText;
  static int level = 0;
  QString thisResult;

  level++;

  while(!(domNode.isNull()))
  {
    if(domNode.isElement())
    {
      domElement = domNode.toElement();
      if(!(domElement.isNull()))
      {
        //debug() << __FUNCTION__ << "isElement" << level << QString(level, ' ').toLocal8Bit().constData() << domElement.tagName().toLocal8Bit().constData();

        QDomNamedNodeMap nma = domElement.attributes();
        int l = nma.length();
        for(  int i=0; i < l; i++ )
        {
          QDomAttr tempa = nma.item(i).toAttr();
          //debug() << __FUNCTION__ << "isElement(attribute)"  << level << QString(level, ' ').toLocal8Bit().constData() << "attribute" << i << tempa.name().toLocal8Bit().constData() << tempa.value().toLocal8Bit().constData();
        }
      }
    }

    if(domNode.isText())
    {
      domText = domNode.toText();
      if(!domText.isNull())
      {
        //debug() << __FUNCTION__ << "isText   " << level << QString(level, ' ').toLocal8Bit().constData() << domText.data().toLocal8Bit().constData();
        thisResult = QString(domText.data().toLocal8Bit().constData());
      }
    }

    TraverseXmlNode(domNode);
    domNode = domNode.nextSibling();
  }

  level--;
  return thisResult;
}
