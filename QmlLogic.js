var component; //Story_2_2
var sprite;

function jsCreateQMLStation(stationName, stationLat, stationLong) {
  var circle = Qt.createQmlObject('import QtLocation 5.6;import QtQuick 2.5; MapCircle {id:station'+stationName+';objectName:"name'+stationName+'"}',mainMap,"test2")
  circle.center.latitude = stationLat
  circle.center.longitude = stationLong
  circle.radius = 500.0
  circle.color = 'green'
  circle.opacity = 0.3

  mainMap.addMapItem(circle)
}


