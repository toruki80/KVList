import QtQuick 2.4
import Insta 1.0

Item {
    width: 400
    height: 400
    property alias mouseArea: mouseArea


    Rectangle {
        anchors.fill: parent
        color: "green"
        radius: Constants.detailViewRadius
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }
}
