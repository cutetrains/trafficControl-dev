import QtQuick 2.3
import QtQuick.Controls 2.12
import QtLocation 5.15
import QtPositioning

Item {
  id: mainItem
  objectName: "nameMainItem"
  PositionSource {
    active: true
    onPositionChanged: {
      console.log(position.coordinate);
    }
  }

  Plugin
  {
    id:googleMapView
    name:"osm"
    //PluginParameter {
    //    name: "osm.mapping.custom.host";
    //    //value: "https://tile.thunderforest.com/cycle/%z/%x/%y.png?apikey=<my key>"
    //    value: "https://tile.openstreetmapView.org"
    //}
  }

  Map {
    id: mapView
    objectName: "namemapview"
    anchors.fill: parent
    plugin:googleMapView
    zoomLevel: (maximumZoomLevel+minimumZoomLevel-3.3)/2
    center: QtPositioning.coordinate(55.9,13.5)

    PinchArea {
      id: pincharea
      //! Holds map object to be controlled
      property var mapView

      //! Holds previous zoom level value
      property double __oldZoom

      anchors.fill: parent

      //! Calculate zoom level
      function calcZoomDelta(zoom, percent) {
          return zoom + Math.log(percent)/Math.log(2)
      }

      //! Save previous zoom level when pinch gesture started
      onPinchStarted: {
          __oldZoom = mapView.zoomLevel
      }

      //! Update map's zoom level when pinch is updating
      onPinchUpdated: {
           mapView.setZoomLevel(calcZoomDelta(__oldZoom, pinch.scale), pinch.center)
      }

      //! Update map's zoom level when pinch is finished
      onPinchFinished: {
          mapView.setZoomLevel(calcZoomDelta(__oldZoom, pinch.scale), pinch.center)
      }

      //! Map's mouse area for implementation of panning in the map
      MouseArea {
        id: mousearea
        //! Property used to indicate if panning the map
        property bool __isPanning: false

        //! Last pressed X and Y position
        property int __lastX: -1
        property int __lastY: -1

        //! Panned distance to distinguish panning from clicks
        property int __pannedDistance: 0

        anchors.fill : parent

        function isPanning() {
            return __isPanning && __pannedDistance > 0;
        }

          //! When pressed, indicate that panning has been started and update saved X and Y values
          onPressed: {
            __isPanning = true
            __lastX = mouse.x
            __lastY = mouse.y
            __pannedDistance = 0
          }

          //! When released, indicate that panning has finished
          onReleased: {
            __isPanning = false
          }

          //! Move the map when panning
          onPositionChanged: {
            if (__isPanning) {
              var dx = mouse.x - __lastX
              var dy = mouse.y - __lastY
              mapView.pan(dx, dy)
              __lastX = mouse.x
              __lastY = mouse.y
              __pannedDistance += Math.abs(dx) + Math.abs(dy);
            }
          }

          onCanceled: {
            __isPanning = false;
          }

          onDoubleClicked: {
            if (!isPanning())
              mapView.center = gps.position.coordinate;
          }

          onWheel: { //USED
            if (wheel.angleDelta.y > 0) {
              mapView.zoomLevel = mapView.zoomLevel + 0.3
            } else {
              mapView.zoomLevel = mapView.zoomLevel - 0.3
            }
          }
        }
    }
  }

  function qmlStationOccupancySlot(stationName, nbrOfTrains, nbrOfPlatforms)
  {
    for (var i = 0; i < mapView.children.length; ++i)
    {
      if(mapView.children[i].objectName === "station_"+stationName)
      {
        mapView.children[i].border.width = 4;
        if(0 === nbrOfTrains){
          mapView.children[i].border.color = "green";
        } else if (nbrOfTrains === nbrOfPlatforms){
          mapView.children[i].border.color = "red";
        } else {
          mapView.children[i].border.color = "yellow";
        }
      }
    }
  }

  function qmlTrackStatusSlot(trackName, nbrOfTrains, trackStatus)
  {
    var foundTrackId = -1;
    var foundStartSignalId = -1;
    var foundEndSignalId = -1;
    var j;
    for (var i = 0; i < mapView.children.length; ++i)
    {
      if(mapView.children[i].objectName === "track_"+trackName) {foundTrackId = i;}
      if(mapView.children[i].objectName === "track_"+trackName+"_START") {foundStartSignalId = i;}
      if(mapView.children[i].objectName === "track_"+trackName+"_END") {foundEndSignalId = i;}
    }
    mapView.children[foundTrackId].line.width = nbrOfTrains+1;
    if ("FREE" === trackStatus){
      mapView.children[foundTrackId].line.color = "green";
      for (j = 0; j < mapView.children.length; ++j) {
        if(mapView.children[j].objectName === "track_"+trackName+"_START") {
          mapView.children[j].destroy();
        }
      }
      for (j = 0; j < mapView.children.length; ++j) {
        if(mapView.children[j].objectName === "track_"+trackName+"_END") {
          mapView.children[j].destroy();
        }
      }
    } else {
      //LOCK SIGNALS NEED TO BE ADDED
      var coordinateStart=mapView.children[foundTrackId].coordinateAt(0);
      var coordinateEnd=mapView.children[foundTrackId].coordinateAt(mapView.children[foundTrackId].pathLength()-1);

      var startMarkLon = 0.95*coordinateStart.longitude+0.05*coordinateEnd.longitude;
      var startMarkLat = 0.95*coordinateStart.latitude+0.05*coordinateEnd.latitude;
      var endMarkLon = 0.05*coordinateStart.longitude+0.95*coordinateEnd.longitude;
      var endtMarkLat = 0.05*coordinateStart.latitude+0.95*coordinateEnd.latitude;
      if(-1 == foundStartSignalId){
        qmlAddTrackSignal(mapView.children[foundTrackId].objectName, "START", startMarkLat, startMarkLon);
        foundStartSignalId = i++;
      }
      if(-1 == foundEndSignalId){
        qmlAddTrackSignal(mapView.children[foundTrackId].objectName, "END", endtMarkLat, endMarkLon);
        foundEndSignalId = i++;
      }
      if ("LOCKED_START" === trackStatus){
        mapView.children[foundTrackId].line.color = "yellow";
        mapView.children[foundStartSignalId].color="red";
        mapView.children[foundEndSignalId].color="green";
      } else if ("LOCKED_END" === trackStatus){
        mapView.children[foundTrackId].line.color = "yellow";
        mapView.children[foundStartSignalId].color="green";
        mapView.children[foundEndSignalId].color="red";
      } else if ("BUSY" === trackStatus || "EMERGENCY" === trackStatus){
        mapView.children[foundTrackId].line.color = "red";
        mapView.children[foundStartSignalId].color="red";
        mapView.children[foundEndSignalId].color="red";
      }
    }
  }


  function qmlAddTrackSignal(name, startOrEnd, lat, lon)
  {
    var circle = Qt.createQmlObject('import QtLocation 5.15; import QtQuick 2.3; MapCircle {id:'
                                     +name+'_'+startOrEnd
                                     +';objectName:"'+name+'_'+startOrEnd + '"}',
                                     mapView,
                                     "test2")
    circle.center.latitude = lat
    circle.center.longitude = lon
    circle.radius = 50.0;
    circle.color = "red"
    circle.opacity = 1;

    mapView.addMapItem(circle)
  }


  function qmlTrainPositionSlot(trainName, latitude, longitude)
  {
    for (var i = 0; i < mapView.children.length; ++i)
    {
      if(mapView.children[i].objectName === "train_"+trainName)
      {
        mapView.children[i].center.latitude = latitude;
        mapView.children[i].center.longitude = longitude;
      }
    }
  }

  function createQMLTrain(trainName) {
    var circle = Qt.createQmlObject('import QtLocation 5.15; import QtQuick 2.3; MapCircle {id:train_'
                                     +trainName
                                     +';objectName:"train_'+trainName+'"}',
                                     mapView,
                                     "test2")
    circle.border.width =1
    circle.border.color = "green"
    circle.center.latitude = 55.7
    circle.center.longitude = 12.8
    circle.radius = 78.0;
    circle.color = "red";
    circle.opacity = 0.7
    mapView.addMapItem(circle)
  }

  function createQMLTrack(trackName, length, coordinates) {
    var polyline = Qt.createQmlObject('import QtLocation 5.15; import QtQuick 2.3; MapPolyline {id:track_'
                                       +trackName
                                       +';objectName:"track_'+trackName+'"}',
                                       mapView,
                                       "test2")
    console.log("ADD QML TRACK")
    var iii;
    polyline.line.color = "green";
    polyline.line.width = 1;
    polyline.opacity=0.8;
    for(iii= 0; iii<coordinates.length; iii++)
    {
      polyline.addCoordinate(QtPositioning.coordinate(coordinates[iii*2],coordinates[iii*2+1]))
    }
    mapView.addMapItem(polyline)
  }

  function createQMLStation(stationName, isJunction, stationLat, stationLong) {
    var circle = Qt.createQmlObject('import QtLocation 5.15; import QtQuick 2.3; MapCircle {id:station_'
                                     +stationName
                                     +';objectName:"station_'
                                     +stationName
                                     +'"}',
                                     mapView,
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
    mapView.addMapItem(circle)
  }
}