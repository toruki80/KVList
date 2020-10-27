import QtQuick 2.14
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10

import "widgets"

Item {
    id: root
    width: 400
    height: 400

    property alias saveButton: saveButton
    property alias cancelButton: cancelButton
    property alias deleteButton: deleteButton
    property alias logoArea: logoArea

    Rectangle {
        id: background
        radius: Constants.detailViewRadius
        anchors.fill: parent
        color: Constants.background_grey
    }

    Text {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60

        id: headline
        text: qsTr("edit contact")
        font: Constants.h4
        color: Constants.orange
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    ScrollView {
        width: parent.width
        height: parent.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: headline.bottom
        anchors.bottom: navrect.top
        contentWidth: content.width // The important part
        contentHeight: content.height // Same
        clip: true // Prevent drawing column outside the scrollview borders

        Item {
            id: content
            width: root.width
            height: column.height

            ColumnLayout {
                id: column
                //                anchors.fill: parent
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 15
                anchors.rightMargin: 15

                MouseArea {
                    id: logoArea
                    width: parent.width / 3
                    height: logoArea.width
                    Layout.alignment: Qt.AlignHCenter

                    RoundImage {
                        anchors.fill: parent
                        border.color: Constants.white
                        backgroundcolor: Constants.white
                        source: currentListItem.logoURL_shadowed
                    }
                }

                Rectangle {
                    Layout.preferredHeight: 1
                    Layout.fillWidth: true
                    color: "black"
                }

                EditEntry {
                    Layout.fillWidth: true
                    name: qsTr("first name")
                    value: currentListItem.firstname
                    onEntryValueChanged: currentListItem.firstname_shadowed = value
                }

                Rectangle {
                    height: 1
                    Layout.fillWidth: true
                    color: "black"
                }

                EditEntry {
                    Layout.fillWidth: true
                    name: qsTr("surname")
                    value: currentListItem.surname
                    onEntryValueChanged: currentListItem.surname_shadowed = value
                }

                Rectangle {
                    height: 1
                    Layout.fillWidth: true
                    color: "black"
                }

                EditEntry {
                    Layout.fillWidth: true
                    name: qsTr("email")
                    value: currentListItem.email
                    onEntryValueChanged: currentListItem.email_shadowed = value
                }

                Rectangle {
                    height: 1
                    Layout.fillWidth: true
                    color: "black"
                }

                EditEntry {
                    Layout.fillWidth: true
                    name: qsTr("phonenumber")
                    value: currentListItem.phonenumber
                    onEntryValueChanged: currentListItem.phonenumber_shadowed = value
                }

                Rectangle {
                    height: 1
                    Layout.fillWidth: true
                    color: "black"
                }

                EditEntry {
                    Layout.fillWidth: true
                    name: qsTr("address")
                    value: currentListItem.address
                    onEntryValueChanged: currentListItem.address_shadowed = value
                }
                Rectangle {
                    height: 1
                    Layout.fillWidth: true
                    color: "black"
                }

                Flow {
                    id: flow
                    Layout.fillWidth: true
                    spacing: 3

                    Repeater {
                        model: currentListItem.activitiesAll
                        delegate: Rectangle {
                            id: delegateRoot
                            property bool isSelected: model.selected
                            color: "white"
                            width: myText.contentWidth + 5
                            height: myText.contentHeight + 5
                            border.color: isSelected ? Constants.edit_activity_active : Constants.edit_activity_inactive
                            border.width: 1
                            radius: 3
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    delegateRoot.isSelected = !delegateRoot.isSelected
                                    model.selected_shadowed = isSelected
                                }
                            }
                            Text {
                                id: myText
                                anchors.centerIn: parent
                                text: model.name
                                font: Constants.h6
                                elide: Text.ElideRight
                                color: isSelected ? Constants.edit_activity_active : Constants.edit_activity_inactive
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: navrect
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 45

        RowLayout {
            anchors.fill: parent

            NavigateButton {
                id: deleteButton
                text: qsTr("delete")
                Layout.fillHeight: true
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            NavigateButton {
                id: cancelButton
                text: qsTr("cancel")
                Layout.fillHeight: true
            }
            NavigateButton {
                id: saveButton
                text: qsTr("save")
                Layout.fillHeight: true
            }
        }
    }
}
