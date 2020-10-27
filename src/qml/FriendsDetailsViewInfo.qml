import QtQuick 2.4
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.12

FriendsDetailsViewInfoForm {
    signal goToPage(string where)

    backButton.onClicked: goToPage("back")
    editButton.onClicked: goToPage("edit")

    mailButton.onClicked: contactPopup.open()
    chatButton.onClicked: contactPopup.open()
    videoButton.onClicked: contactPopup.open()
    callButton.onClicked: contactPopup.open()

    Popup {
        id: contactPopup
        parent: ApplicationWindow.overlay
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        width: parent.width
        height: parent.height

        background: Rectangle {
            color: "#90000000"
        }

        contentData: MouseArea {
            anchors.fill: parent
            onClicked: {
                currentListItem.ENTRY.updateLastSeenStatus(true)
                currentListItem.MODEL.serialize()
                contactPopup.close()
            }
            ColumnLayout {
                anchors.fill: parent
                Item{
                    Layout.fillHeight: true
                }

                BusyIndicator {
                    id: busyIndicator
                    implicitWidth: 100
                    implicitHeight: 100
                    visible: running
                    running: true
                    Layout.fillWidth: true
                    Component.onCompleted: {
                        contentItem.pen = "white"
                        contentItem.fill = "white"
                    }
                }

                Text {
                    text: "make contact"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "white"
                    font: Constants.overlay
                    Layout.fillWidth: true
                }

                Item{
                    Layout.fillHeight: true
                }
            }
        }
    }
}
