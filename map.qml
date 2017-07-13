import QtQuick 2.5
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.5

import "QmlLogic.js" as Logic

Item {
  id: mainItemGustaf
  objectName: "nameMainItem"
  PositionSource {
    active: true
    onPositionChanged: {
      console.log(position.coordinate);
    }
  }

  Map {
    id: mainMap
    objectName: "nameMainMap"
    anchors.fill: parent
    plugin: Plugin { name: "osm" }
    zoomLevel: (maximumZoomLevel+minimumZoomLevel-3.3)/2
    center: QtPositioning.coordinate(55.9,13.5)
  }

  function qmlStationOccupancySlot(stationName, nbrOfTrains, nbrOfPlatforms)
  {
    for (var i = 0; i < mainMap.children.length; ++i)
    {
      if(mainMap.children[i].objectName === "station_"+stationName)
      {
        mainMap.children[i].border.width = 4;
        if(0 === nbrOfTrains){
          mainMap.children[i].border.color = "green";
        } else if (nbrOfTrains === nbrOfPlatforms){
          mainMap.children[i].border.color = "red";
        } else {
          mainMap.children[i].border.color = "yellow";
        }
      }
    }
  }

  function qmlTrackStatusSlot(trackName, nbrOfTrains, trackStatus)
  {
    for (var i = 0; i < mainMap.children.length; ++i)
    {
      if(mainMap.children[i].objectName === "track_"+trackName)
      {
        mainMap.children[i].line.width = nbrOfTrains+1;
        if ("AVAILABLE" == trackStatus){
          if (0 == nbrOfTrains){
            mainMap.children[i].line.color = "green";
          } else {
            mainMap.children[i].line.color = "yellow";
          }
        } else if ("BUSY" == trackStatus || "EMERGENCY" == trackStatus){
          mainMap.children[i].line.color = "red";
        } else if ("SIGNAL_ERROR" == trackStatus){
          mainMap.children[i].line.color = "darkGray"
        }
      }
    }
  }

  function qmlTrainPositionSlot(trainName, latitude, longitude)
  {
    for (var i = 0; i < mainMap.children.length; ++i)
    {
      if(mainMap.children[i].objectName === "train_"+trainName)
      {
        mainMap.children[i].center.latitude = latitude;
        mainMap.children[i].center.longitude = longitude;
      }
    }
  }

  function createQMLTrain(trainName) {
    var circle = Qt.createQmlObject('import QtLocation 5.6; import QtQuick 2.5; MapCircle {id:train_'
                                     +trainName
                                     +';objectName:"train_'+trainName+'"}',
                                     mainMap,
                                     "test2")
    circle.border.width =1
    circle.border.color = "green"
    circle.center.latitude = 55.7
    circle.center.longitude = 12.8
    circle.radius = 78.0;
    circle.color = "red";
    circle.opacity = 1
    mainMap.addMapItem(circle)
  }

  function createQMLTrack(trackName, length, coordinates) {
    var polyline = Qt.createQmlObject('import QtLocation 5.6; import QtQuick 2.5; MapPolyline {id:track_'
                                       +trackName
                                       +';objectName:"track_'+trackName+'"}',
                                       mainMap,
                                       "test2")
    var iii;
    polyline.line.color = "green";
    polyline.line.width = 1;
    polyline.opacity=0.8;
    for(iii= 0; iii<coordinates.length; iii++)
    {
      polyline.addCoordinate(QtPositioning.coordinate(coordinates[iii*2],coordinates[iii*2+1]))
    }
    mainMap.addMapItem(polyline)
  }

  function createQMLStation(stationName, isJunction, stationLat, stationLong) {
    var circle = Qt.createQmlObject('import QtLocation 5.6; import QtQuick 2.5; MapCircle {id:station_'
                                     +stationName
                                     +';objectName:"station_'
                                     +stationName
                                     +'"}',
                                     mainMap,
                                     "test2")
    circle.center.latitude = stationLat
    circle.center.longitude = stationLong
    if(isJunction){
      circle.radius = 250.0;
      circle.color = "gray";
    }
    else
    {
      circle.radius = 1000.0
      circle.color = 'green'
    }
    circle.opacity = 0.3
    mainMap.addMapItem(circle)
  }
}
