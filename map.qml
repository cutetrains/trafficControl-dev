import QtQuick 2.3
import QtQuick.Controls 2.12
import QtLocation 5.15
import QtPositioning 5.15

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
    var foundTrackId = -1;
    var foundStartSignalId = -1;
    var foundEndSignalId = -1;
    var j;
    for (var i = 0; i < mainMap.children.length; ++i)
    {
      if(mainMap.children[i].objectName === "track_"+trackName) {foundTrackId = i;}
      if(mainMap.children[i].objectName === "track_"+trackName+"_START") {foundStartSignalId = i;}
      if(mainMap.children[i].objectName === "track_"+trackName+"_END") {foundEndSignalId = i;}
    }
    mainMap.children[foundTrackId].line.width = nbrOfTrains+1;
    if ("FREE" === trackStatus){
      mainMap.children[foundTrackId].line.color = "green";
      for (j = 0; j < mainMap.children.length; ++j) {
        if(mainMap.children[j].objectName === "track_"+trackName+"_START") {
          mainMap.children[j].destroy();
        }
      }
      for (j = 0; j < mainMap.children.length; ++j) {
        if(mainMap.children[j].objectName === "track_"+trackName+"_END") {
          mainMap.children[j].destroy();
        }
      }
    } else {
      //LOCK SIGNALS NEED TO BE ADDED
      var coordinateStart=mainMap.children[foundTrackId].coordinateAt(0);
      var coordinateEnd=mainMap.children[foundTrackId].coordinateAt(mainMap.children[foundTrackId].pathLength()-1);

      var startMarkLon = 0.95*coordinateStart.longitude+0.05*coordinateEnd.longitude;
      var startMarkLat = 0.95*coordinateStart.latitude+0.05*coordinateEnd.latitude;
      var endMarkLon = 0.05*coordinateStart.longitude+0.95*coordinateEnd.longitude;
      var endtMarkLat = 0.05*coordinateStart.latitude+0.95*coordinateEnd.latitude;
      if(-1 == foundStartSignalId){
        qmlAddTrackSignal(mainMap.children[foundTrackId].objectName, "START", startMarkLat, startMarkLon);
        foundStartSignalId = i++;
      }
      if(-1 == foundEndSignalId){
        qmlAddTrackSignal(mainMap.children[foundTrackId].objectName, "END", endtMarkLat, endMarkLon);
        foundEndSignalId = i++;
      }
      if ("LOCKED_START" === trackStatus){
        mainMap.children[foundTrackId].line.color = "yellow";
        mainMap.children[foundStartSignalId].color="red";
        mainMap.children[foundEndSignalId].color="green";
      } else if ("LOCKED_END" === trackStatus){
        mainMap.children[foundTrackId].line.color = "yellow";
        mainMap.children[foundStartSignalId].color="green";
        mainMap.children[foundEndSignalId].color="red";
      } else if ("BUSY" === trackStatus || "EMERGENCY" === trackStatus){
        mainMap.children[foundTrackId].line.color = "red";
        mainMap.children[foundStartSignalId].color="red";
        mainMap.children[foundEndSignalId].color="red";
      }
    }
  }


  function qmlAddTrackSignal(name, startOrEnd, lat, lon)
  {
    var circle = Qt.createQmlObject('import QtLocation 5.15; import QtQuick 2.3; MapCircle {id:'
                                     +name+'_'+startOrEnd
                                     +';objectName:"'+name+'_'+startOrEnd + '"}',
                                     mainMap,
                                     "test2")
    circle.center.latitude = lat
    circle.center.longitude = lon
    circle.radius = 50.0;
    circle.color = "red"
    circle.opacity = 1;

    mainMap.addMapItem(circle)
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
    var circle = Qt.createQmlObject('import QtLocation 5.15; import QtQuick 2.3; MapCircle {id:train_'
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
    circle.opacity = 0.7
    mainMap.addMapItem(circle)
  }

  function createQMLTrack(trackName, length, coordinates) {
    var polyline = Qt.createQmlObject('import QtLocation 5.15; import QtQuick 2.3; MapPolyline {id:track_'
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
    var circle = Qt.createQmlObject('import QtLocation 5.15; import QtQuick 2.3; MapCircle {id:station_'
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
