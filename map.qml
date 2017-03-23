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
    zoomLevel: 1

    Component.onCompleted: {
      console.log("Initiated mainMap")
      console.log(Logic.testVariable)
    }

    //TODO: Delete the hard coded elements
    MapCircle {
      id: mainMapCircle1
      objectName: "nameMainMapCircle1"
      center {
        latitude: 55.60985893200806
        longitude: 13.00118644566413
      }
      radius: 50.0
      color: 'red'
      border.width: 3
      opacity: 0.3
    }


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
  function createQMLStationFromSprite(stationName, isJunction, stationLat, stationLong) {
    console.log("INFO   : createQMLStation called with these arguments: ", stationName, isJunction, stationLat, stationLong);
    Logic.jsCreateQMLStation(stationName,stationLat,stationLong)
  }
}

