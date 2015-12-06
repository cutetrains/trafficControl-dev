import QtQuick 2.0
import QtPositioning 5.2
import QtLocation 5.2

Item {
  PositionSource {
      active: true
      onPositionChanged: {
          console.log(position.coordinate);
      }
  }

  Map {
     id: map
     anchors.fill: parent
     plugin: Plugin { name: "osm" }
     center: QtPositioning.coordinate(55.6,13)
     zoomLevel: 15

     MapPolyline {
        line.width: 3
        line.color: 'green'
        path: [
      {  latitude: 55.59352720563821, longitude: 13.00096502906901 },
      {  latitude: 55.59478578744516, longitude: 13.0001047610863 },
      {  latitude: 55.59584153256193, longitude: 12.99895608500405 },
      {  latitude: 55.59708025106826, longitude: 12.99698496626038 },
      {  latitude: 55.59810456329647, longitude: 12.994872748726 },
      {  latitude: 55.59908092573248, longitude: 12.99315089646847 },
      {  latitude: 55.60029773317108, longitude: 12.99159948266996 },
      {  latitude: 55.60179083598059, longitude: 12.9901983340431 },
      {  latitude: 55.60336289793883, longitude: 12.98944544460986 },
      {  latitude: 55.60508465278291, longitude: 12.98944762062867 },
      {  latitude: 55.60670470985342, longitude: 12.99016230283142 },
      {  latitude: 55.60777164012884, longitude: 12.99162346122634 },
      {  latitude: 55.60851310015489, longitude: 12.99412169073328 },
      {  latitude: 55.60905807131994, longitude: 12.99689436869777 },
      {  latitude: 55.60945552275268, longitude: 12.99929500070747 },
      {  latitude: 55.60981997270764, longitude: 13.00125368558942 }
        ]
    }
  }

}

