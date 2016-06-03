.pragma library // Shared game state
.import QtQuick 2.0 as QQ

var testVariable=0;
//public class QmlLogic.js {

//}

function jsCreateQMLStation(stationName, stationLat, stationLong) {
    console.log("jsCreateQMLStation called with arguments: ", stationName, stationLat, stationLong);
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
    }
