import QtQuick 2.4
import QtPositioning 5.2
import QtLocation 5.5

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

  Map {
     id: mainMap
     objectName: "nameMainMap"
     anchors.fill: parent
     plugin: Plugin { name: "osm" }
     center: QtPositioning.coordinate(55.6,13)
     zoomLevel: 15

    Component.onCompleted: {
        console.log("Initiated mainMap")
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

     //MapCircle  { id: sGunnesbo; objectName: "GunnesboName"; center { latitude: 55.71606862405453; longitude: 13.19645933296118} radius:50.0; color: "red"; opacity:0.3 ;border.width: 3}

     MapCircle {
        id: mainMapCircle2
        objectName: "nameMainMapCircle2"
        center {
            latitude: 55.6
            longitude: 13
        }
        radius: 100.0
        color: 'blue'
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
     function createQMLStation(stationName, stationLat, stationLong) {
        console.log("INFO   : createQMLStation called with arguments: ", stationName, stationLat, stationLong);
        Logic.jsCreateQMLStation(stationName,stationLat,stationLong)
        /*
        //textLabel1.color = msg
        //eval('MapCircle  { id: sGunnesbo; objectName: "GunnesboName"; center { latitude: 55.71606862405453; longitude: 13.19645933296118} radius:500.0; color: "red"; opacity:0.3 ;border.width: 3}')
        var str="";
        str='import QtQuick 2.4 \n  import QtPositioning 5.2 \n import QtLocation 5.5\n MapCircle { \n id: s' + stationName +'\n objectName: "' + stationName+'Name"\n center { latitude: ' + stationLat +'\n longitude: ' + stationLong + '} \n radius:50.0\n color: "red"\n opacity:0.3 \n border.width: 3}'
        //str='MapCircle { ; id: s' + stationName +'; objectName: "' + stationName+'Name"; center { latitude: ' + stationLat +'; longitude: ' + stationLong + '} radius:50.0; color: "red"; opacity:0.3 ;border.width: 3}'
        //console.log('MapCircle { id: s' + stationName +'; objectName: "' + stationName+'Name"; center { latitude: ' + stationLat +'; longitude: ' + stationLong + '} radius:50.0; color: "red"; opacity:0.3 ;border.width: 3}')
        console.log(str)
        //eval(str)
        //eval('import QtQuick 2.3; MapCircle { id: s' + stationName +'; objectName: \"' + stationName+'Name\"; center { latitude: ' + stationLat +'; longitude: ' + stationLong + '} radius:50.0; color: \'red\'; opacity=0.3 ;border.width: 3}')

        //eval('MapCircle {\n id: s' + stationName +'\n objectName: \"' + stationName+'Name\"\n center { \n latitude: ' + stationLat +'\n longitude: ' + stationLong + '\n} \n radius:500.0\n color: \'red\'\n opacity=0.3 \nborder.width: 3 \n }')

        //eval('MapCircle { id: sGunnesbo; objectName: "GunnesboName"; center { latitude: 55.71606862405453; longitude: 13.18645933296118} radius:50.0; color: "red"; opacity:0.3 ;border.width: 3}')

        //var newObject = Qt.createQmlObject('MapCircle { objectName: \"' + stationName+'Name\" center { \n latitude: ' + stationLat +' longitude: ' + stationLong + '}  radius:500.0 color: \'red\' opacity:0.3 border.width: 3  }', mainMap,'firstmap')

        //console.log(myNum)
        var newObject = Qt.createQmlObject(str, mainMap, 'firstmap')
        if (newObject === null)
        {
            console.log("Object doesn't exist!!!")
        }
        else
        {
            console.log("Object exists!!!")
        }

        return(stationName)
        */
    }

}

