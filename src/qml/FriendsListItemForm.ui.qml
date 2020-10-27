import QtQuick 2.4
import QtGraphicalEffects 1.10
import "widgets"

Item {
    property alias mousearea: mousearea
    MouseArea {
        id: mousearea
        anchors.fill: parent
    }
    Rectangle {
        id: bgleft
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.height + 10
        radius: 4
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#fb92a0"
            }
            GradientStop {
                position: 1.0
                color: "#fc9360"
            }
        }

        Item {
            anchors.fill: parent
            anchors.rightMargin: 10
            RoundImage {
                id: photo

                anchors.fill: parent
                anchors.margins: 10
                border.color: "white"
                backgroundcolor: "white"
                source: model.logoURL
            }
        }
    }
    Rectangle {
        id: bgright
        anchors.left: bgleft.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: parent.height
        radius: bgleft.radius
        color: Constants.background_grey

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10
            Text {
                id: name
                width: parent.width
                text: model.displayName_ns
                font: Constants.h4
                color: Constants.orange
            }

            Text {
                id: statusText
                width: parent.width
                text: "last seen: " + model.displayLastseen_ns
                font: Constants.h6
                color: Constants.orange
            }
        }
    }

    Rectangle {
        anchors.left: bgleft.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: bgright.left
        anchors.leftMargin: -bgleft.radius
        anchors.rightMargin: -bgright.radius
        color: Constants.background_grey
    }
}
