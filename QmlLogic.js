var component; //Story_2_2
var sprite;

function jsCreateQMLStation(stationName, isJunction, stationLat, stationLong) {
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

function jsCreateQMLTrack(trackName, length, coordinates) {
  var polyline = Qt.createQmlObject('import QtLocation 5.6; import QtQuick 2.5; MapPolyline {id:track_'
                                     +trackName
                                     +';objectName:"track_'+trackName+'"}',
                                     mainMap,
                                     "test2")
  var iii;
  polyline.line.color = "green";
  polyline.line.width = 1;
  polyline.opacity=0.5;
  for(iii= 0; iii<coordinates.length; iii++)
  {
    polyline.addCoordinate(QtPositioning.coordinate(coordinates[iii*2],coordinates[iii*2+1]))
  }
  mainMap.addMapItem(polyline)
}


function jsCreateQMLTrain(trainName) {
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
