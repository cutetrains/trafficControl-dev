import QtQuick 2.5
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.5

import "resources/QmlLogic.js" as Logic

Item {
  id: mainItemGustaf
  objectName: "nameMainItem"
  PositionSource {
      active: true
      onPositionChanged: {
          console.log(position.coordinate);
      }
  }

/*  Map {
    id: mainMap
    anchors.centerIn: parent
    anchors.fill: parent
    center: QtPositioning.coordinate(55.6,13)
    plugin: Plugin {
       name: "osm"
    }
  }
}*/



  Map {
     id: mainMap
     objectName: "nameMainMap"
     anchors.fill: parent
     plugin: Plugin { name: "osm" }
     //center: QtPositioning.coordinate(13.0,55.6)//55.6,13
     zoomLevel: 1

    Component.onCompleted: {
        console.log("Initiated mainMap")
        console.log(Logic.testVariable)
        //Try to call a function and create a station!
        Logic.jsCreateQMLStation("hejsan", false, 55.61, 13.01)
        //var myNum = 15
        var newObject

    }

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

     function createQMLStationFromSprite(stationName, isJunction, stationLat, stationLong) {
        console.log("INFO   : createQMLStation called with these arguments: ", stationName, isJunction, stationLat, stationLong);
        Logic.jsCreateQMLStation(stationName,stationLat,stationLong)

    }

}

