import QtQuick 2.0
import QtGraphicalEffects 1.10
import ".."

Item {
    id: root
    property alias source: img.source
    property alias border: border.border
    property alias backgroundcolor: circle.color

    Rectangle {
        id: circle
        anchors.fill: parent
        radius: 180
        visible: true
        color: "green"
    }

    Image {
        id: img
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: "../icons/default_user.png"
        visible: false
    }

    OpacityMask {
        anchors.fill: parent
        source: img
        maskSource: circle
    }

    Rectangle {
        id: border
        anchors.fill: parent
        border.width: 2
        border.color: Constants.orange
        color: "transparent"
        radius: 180
    }
}
