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
    console.log("In qmlStationOccupancySlot, station: " + stationName);
    for (var i = 0; i < mainMap.children.length; ++i)
    {
      console.log(mainMap.children[i].objectName);
      if(mainMap.children[i].objectName === "name"+stationName)
      {
        console.log("child found");
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

  Map {
    id: mainMap
    objectName: "nameMainMap"
    anchors.fill: parent
    plugin: Plugin { name: "osm" }
    zoomLevel: 1

    Component.onCompleted: {
      console.log("Initiated mainMap")
    }

    //TODO: Delete the hard coded elements
    MapPolyline {
      id: mainMapPoly
      objectName:"nameMainMapPoly"
      line.width: 3
      line.color: 'green'
      path: [
        {  latitude: 55.59352720563821, longitude: 13.00096502906901 },
        {  latitude: 55.60508465278291, longitude: 12.98944762062867 },
        {  latitude: 55.60981997270764, longitude: 13.00125368558942 }
            ]
    }


  }

  //TODO: Create generic

  function createQMLStation(stationName, isJunction, stationLat, stationLong) {
    Logic.jsCreateQMLStation(stationName,stationLat,stationLong)
  }

}

