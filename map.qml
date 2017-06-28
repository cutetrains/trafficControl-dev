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

  function qmlStationOccupancySlot(stationName, nbrOfTrains, nbrOfPlatforms)
  {
    for (var i = 0; i < mainMap.children.length; ++i)
    {
      if(mainMap.children[i].objectName === "name_"+stationName)
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
      console.log("in qmlTrackStatusSlot" + trackName)
      if(mainMap.children[i].objectName === "track_"+trackName)
      {
        console.log("found item! "  + trackName + " Status: " + trackStatus)
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

  Map {
    id: mainMap
    objectName: "nameMainMap"
    anchors.fill: parent
    plugin: Plugin { name: "osm" }
    zoomLevel: 1

  }

  function createQMLStation(stationName, isJunction, stationLat, stationLong) {
    Logic.jsCreateQMLStation(stationName,stationLat,stationLong)
  }

  function createQMLTrack(trackName, length, coordinates) {
    Logic.jsCreateQMLTrack(trackName, length, coordinates)
  }

}

