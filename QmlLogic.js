var component; //Story_2_2
var sprite;

function jsCreateQMLStation(stationName, stationLat, stationLong) {
  //TODO: remove stationSprite
  console.log("jsCreateQMLStation called with arguments: ", stationName, stationLat, stationLong);
  //TODO: remove stationSprite

  var circle = Qt.createQmlObject('import QtLocation 5.6;import QtQuick 2.5; MapCircle {}',mainMap,"test2")
  circle.center.latitude = stationLat
  circle.center.longitude = stationLong
  //TODO: Set new name of circle so that can be modified later

  circle.radius = 500.0
  circle.color = 'green'
  circle.opacity = 0.3
  circle.border.width = 3
  mainMap.addMapItem(circle)

  //TODO: Add error handling
}
