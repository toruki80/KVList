import QtQuick 2.4
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.10
import Insta 1.0
import "widgets"

Item {
    id: root
    width: 500
    height: 600
    property alias viewInfo: viewInfo
    property alias flipable: flipable
    property alias backloader: backloader
    property var currentListItem: null

    readonly property int contactButtonSize: 22
    readonly property int navigateButtonSize: 22

    Flipable {
        id: flipable

        property bool flipped: false

        anchors.fill: parent
        anchors.margins: 30

        front: FriendsDetailsViewInfo {
            id: viewInfo
            anchors.fill: parent
        }

        back: Loader {
            id: backloader
            anchors.fill: parent
        }

        transform: Rotation {
                id: rotation
                origin.x: flipable.width/2
                origin.y: flipable.height/2
                axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
                angle: 0    // the default angle
            }

            states: State {
                name: "back"
                PropertyChanges { target: rotation; angle: 180 }
                when: flipable.flipped
            }

            transitions: Transition {
                NumberAnimation { target: rotation; property: "angle"; duration: 80 }
            }
    }
}
