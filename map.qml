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

  function createQMLStation(stationName, isJunction, stationLat, stationLong) {
    Logic.jsCreateQMLStation(stationName, isJunction, stationLat,stationLong)
  }

  function createQMLTrack(trackName, length, coordinates) {
    Logic.jsCreateQMLTrack(trackName, length, coordinates)
  }

  function createQMLTrain(trainName) {
    console.log("createQMLTrain: Adding Train to QML map")
    Logic.jsCreateQMLTrain(trainName)
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
    console.log("In qmlTrainPositionSlot "+ trainName + "  " + latitude + "  " + longitude)
    for (var i = 0; i < mainMap.children.length; ++i)
    {
      if(mainMap.children[i].objectName === "train_"+trainName)
      {
        console.log("Train found!")
        mainMap.children[i].center.latitude = latitude;
        mainMap.children[i].center.longitude = longitude;
      }
    }
  }
}
