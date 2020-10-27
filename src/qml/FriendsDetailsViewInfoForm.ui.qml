import QtQuick 2.4
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10
import Insta 1.0
import "widgets"

Item {
    id: content
    width: 800
    height: 400
    property alias backButton: backButton
    property alias editButton: editButton

    property alias mailButton: mailButton
    property alias callButton: callButton
    property alias chatButton: chatButton
    property alias videoButton: videoButton


    Rectangle {
        id: bgtop
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        color: "red"
        radius: Constants.detailViewRadius
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

        height: infoText.height + 40
    }

    Rectangle {
        id: bgbottom
        anchors.top: bgtop.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        color: Constants.background_grey
        radius: Constants.detailViewRadius
    }

    Rectangle {
        id: bgcenter
        anchors.top: bgtop.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bgbottom.top
        anchors.topMargin: -Constants.detailViewRadius
        anchors.bottomMargin: -Constants.detailViewRadius
        color: bgbottom.color
    }

    Column {
        id: infoText
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Item {
            width: 1
            height: 13
        }

        RowLayout {
            width: parent.width
            Button {
                id: backButton

                icon.source: "icons/button_back.png"
                icon.color: Constants.white
                icon.width: navigateButtonSize
                icon.height: navigateButtonSize

                background: null
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: editButton

                icon.source: "icons/button_edit.png"
                icon.color: Constants.white
                icon.width: navigateButtonSize
                icon.height: navigateButtonSize

                background: null
            }
        }

        RoundImage {
            width: parent.width / 3
            height: width
            anchors.horizontalCenter: parent.horizontalCenter
            border.color: Constants.white
            backgroundcolor: Constants.white
            source: currentListItem.logoURL
        }

        Item{
            width: 1
            height: 15
        }

        Text {
            id: name
            text: currentListItem.firstname + ' ' + currentListItem.surname
            anchors.horizontalCenter: parent.horizontalCenter
            font: Constants.h2
            color: Constants.white
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                id: callButton
                icon.width: contactButtonSize
                icon.height: contactButtonSize
                display: AbstractButton.IconOnly
                icon.source: "icons/button_audiocall.png"
                icon.color: Constants.white
                background: null
            }
            Button {
                id: mailButton
                icon.width: contactButtonSize
                icon.height: contactButtonSize
                display: AbstractButton.IconOnly
                icon.source: "icons/button_email.png"
                icon.color: Constants.white
                background: null
            }
            Button {
                id: chatButton
                icon.width: contactButtonSize
                icon.height: contactButtonSize
                display: AbstractButton.IconOnly
                icon.source: "icons/button_chat.png"
                icon.color: Constants.white
                background: null
            }
            Button {
                id: videoButton
                icon.width: contactButtonSize
                icon.height: contactButtonSize
                display: AbstractButton.IconOnly
                icon.source: "icons/button_videocall.png"
                icon.color: Constants.white
                background: null
            }
        }
    }

    ColumnLayout {
        id: column
        anchors.top: bgbottom.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 5
        spacing: 15

        Text {
            text: "Activities"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font: Constants.h2
            color: Constants.orange
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            Flow {
                id: activitiesFlow
                x:column.x +10
                width: column.width - 20
                spacing: 10

                Repeater {
                    model: currentListItem.activitiesSelected
                    delegate: activityRect
                }
            }
        }

        Item
        {
            width: 1
            height: 10
        }
    }

    Component {
        id: activityRect

        Rectangle {
            color: "white"
            width: contentRow.width
            height: contentRow.height
            radius: 10

            Row {
                id: contentRow
                spacing: 5
                Button {
                    id: button
                    display: AbstractButton.IconOnly
                    icon.source: model.iconRes
                    icon.color: Constants.orange
                    background: null
                }
                Text {
                    text: model.name
                    font: Constants.h6
                    elide: Text.ElideRight
                    verticalAlignment: Qt.AlignVCenter
                    height: button.height
                }
                Item {
                    height: 1
                    width: 10
                }
            }
        }
    }
}
