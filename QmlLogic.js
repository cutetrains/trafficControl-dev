var component; //Story_2_2
var sprite;

function jsCreateQMLStation(stationName, stationLat, stationLong) {
  var circle = Qt.createQmlObject('import QtLocation 5.6; import QtQuick 2.5; MapCircle {id:station'
                                   +stationName
                                   +';objectName:"name'
                                   +stationName
                                   +'"}',
                                   mainMap,
                                   "test2")
  circle.center.latitude = stationLat
  circle.center.longitude = stationLong
  circle.radius = 1000.0
  circle.color = 'green'
  circle.opacity = 0.3

  mainMap.addMapItem(circle)
}

function jsCreateQMLTrack(trackName, length, coordinates) {
  var polyline = Qt.createQmlObject('import QtLocation 5.6; import QtQuick 2.5; MapPolyline {id:track'
                                     +trackName
                                     +';objectName:"track_'+trackName+'"}',
                                     mainMap,
                                     "test2")
  polyline.line.color = "green";
  polyline.line.width = 2;
  polyline.path = [
        {  latitude: coordinates[0], longitude: coordinates[1] },
        {latitude: coordinates[coordinates.length-2], longitude: coordinates[coordinates.length-1]}
            ]

  mainMap.addMapItem(polyline)
}

