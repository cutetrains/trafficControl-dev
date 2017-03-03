.pragma library // Shared game state
.import QtQuick 2.5 as QQ
//.import "map.qml"// FIND GOOD URI
var component; //Story_2_2
var sprite;

//var testVariable=123;
//public class QmlLogic.js {

//}

function jsCreateQMLStation(stationName, stationLat, stationLong) {
    console.log("jsCreateQMLStation called with arguments: ", stationName, stationLat, stationLong);
    //QQmlEngine engine;//TEST BY EGUSBRA
    component = Qt.createComponent("qrc:Traffic/qml/stationSprite.qml");
    if (component.status == Component.Ready)
        finishCreation();
    else
        component.statusChanged.connect(finishCreation);
    return(stationName);
}

function finishCreation() {
    if (component.status == Component.Ready) {
        //sprite = component.createObject(appWindow, {"x": 100, "y": 100});
        sprite = component.createObject(nameMainItem, {"center.latitude": stationLat, "center.longitude": stationLong});
        if (sprite == null) {
            // Error Handling
            console.log("Error creating object");
        }
    } else if (component.status == Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString());
    }
}
